#include "memorybaselinereader.h"
#include "../structures/system.h"

#include "../util/aologger.h"
#include "../util/stopwatch.h"

#include <casacore/ms/MeasurementSets/MeasurementSet.h>

#include <vector>

using namespace casacore;

void MemoryBaselineReader::clear()
{
	for(std::map<BaselineID, Result*>::iterator i=_baselines.begin(); i!=_baselines.end(); ++i)
	{
		// They don't all have to contain objects, but will be zero otherwise so safe to delete right away
		delete i->second;
	}
	_baselines.clear();
	_areFlagsChanged = false;
	_isRead = false;
}

void MemoryBaselineReader::PerformReadRequests()
{
	readSet();
	
	for(size_t i=0;i!=_readRequests.size();++i)
	{
		const ReadRequest &request = _readRequests[i];
		BaselineID id;
		id.antenna1 = request.antenna1;
		id.antenna2 = request.antenna2;
		id.spw = request.spectralWindow;
		id.sequenceId = request.sequenceId;
		std::map<BaselineID, Result*>::const_iterator requestedBaselineIter = _baselines.find(id);
		if(requestedBaselineIter == _baselines.end())
		{
			std::ostringstream errorStr;
			errorStr <<
				"Exception in PerformReadRequests(): requested baseline is not available in measurement set "
				"(antenna1=" << request.antenna1 << ", antenna2=" << request.antenna2 << ", "
				"spw=" << request.spectralWindow << ", sequenceId=" << request.sequenceId << ")";
			throw std::runtime_error(errorStr.str());
		}
		else
			_results.push_back(*requestedBaselineIter->second);
	}
	
	_readRequests.clear();
}

void MemoryBaselineReader::readSet()
{
	if(!_isRead)
	{
		Stopwatch watch(true);
		
		initializeMeta();
	
		casacore::Table &table = *Table();
		
		ROScalarColumn<int>
			ant1Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA1)),
			ant2Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA2)),
			dataDescIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::DATA_DESC_ID)),
			fieldIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FIELD_ID));
		ROScalarColumn<double>
			timeColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::TIME));
		ROArrayColumn<casacore::Complex>
			dataColumn(table, DataColumnName());
		ROArrayColumn<bool>
			flagColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FLAG));
		ROArrayColumn<double>
			uvwColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::UVW));
		
		size_t
			antennaCount = Set().AntennaCount(),
			polarizationCount = PolarizationCount();
			
		size_t bandCount = Set().BandCount();
		size_t sequenceCount = Set().SequenceCount();
		
		std::vector<size_t> dataIdToSpw;
		Set().GetDataDescToBandVector(dataIdToSpw);
		
		std::vector<BandInfo> bandInfos(bandCount);
		for(size_t b=0; b!=bandCount; ++b)
			bandInfos[b] = Set().GetBandInfo(b);
		
		// Initialize the look-up matrix
		// to quickly access the elements (without the map-lookup)
		typedef Result* MatrixElement;
		typedef std::vector<MatrixElement> MatrixRow;
		typedef std::vector<MatrixRow> BaselineMatrix;
		typedef std::vector<BaselineMatrix> BaselineCube;
		
		BaselineCube baselineCube(sequenceCount * bandCount);
		
		for(size_t s=0; s!=sequenceCount; ++s)
		{
			for(size_t b=0; b!=bandCount; ++b)
			{
				BaselineMatrix &matrix = baselineCube[s*bandCount + b];
				matrix.resize(antennaCount);
				
				BandInfo band = Set().GetBandInfo(0);
				for(size_t a1=0;a1!=antennaCount;++a1)
				{
					matrix[a1].resize(antennaCount);
					for(size_t a2=0;a2!=antennaCount;++a2)
						matrix[a1][a2] = 0;
				}
			}
		}
		
		// The actual reading of the data
		AOLogger::Debug << "Reading the data...\n";
		
		double prevTime = -1.0;
		size_t curTimeIndex = size_t(0);
		unsigned rowCount = table.nrow();
		
		casacore::Array<casacore::Complex> dataArray;
		casacore::Array<bool> flagArray;
		size_t prevFieldId = size_t(-1), sequenceId = size_t(-1);
		for(unsigned rowIndex = 0;rowIndex < rowCount;++rowIndex)
		{
			size_t fieldId = fieldIdColumn(rowIndex);
			if(fieldId != prevFieldId)
			{
				prevFieldId = fieldId;
				sequenceId++;
				prevTime = -1.0;
			}
			const std::map<double, size_t>
				&observationTimes = ObservationTimes(sequenceId);
			double time = timeColumn(rowIndex);
			if(time != prevTime)
			{
				curTimeIndex = observationTimes.find(time)->second;
				prevTime = time;
			}
			
			size_t ant1 = ant1Column(rowIndex);
			size_t ant2 = ant2Column(rowIndex);
			size_t spw = dataIdToSpw[dataDescIdColumn(rowIndex)];
			size_t spwFieldIndex = spw + sequenceId * bandCount;
			if(ant1 > ant2) std::swap(ant1, ant2);
			
			Result *result = baselineCube[spwFieldIndex][ant1][ant2];
			if(result == 0)
			{
				const size_t timeStepCount = observationTimes.size();
				result = new Result();
				for(size_t p=0;p!=polarizationCount;++p) {
					result->_realImages.push_back(Image2D::CreateZeroImagePtr(timeStepCount, Set().FrequencyCount(spw)));
					result->_imaginaryImages.push_back(Image2D::CreateZeroImagePtr(timeStepCount, Set().FrequencyCount(spw)));
					result->_flags.push_back(Mask2D::CreateSetMaskPtr<true>(timeStepCount, Set().FrequencyCount(spw)));
				}
				result->_bandInfo = bandInfos[spw];
				result->_uvw.resize(timeStepCount);
				baselineCube[spwFieldIndex][ant1][ant2] = result;
			}
			
			dataArray = dataColumn.get(rowIndex);
			flagArray = flagColumn.get(rowIndex);
			
			Array<double> uvwArray = uvwColumn.get(rowIndex);
			Array<double>::const_iterator uvwPtr = uvwArray.begin();
			UVW uvw;
			uvw.u = *uvwPtr; ++uvwPtr;
			uvw.v = *uvwPtr; ++uvwPtr;
			uvw.w = *uvwPtr;
			result->_uvw[curTimeIndex] = uvw;
			
			for(size_t p=0;p!=polarizationCount;++p)
			{
				Array<Complex>::const_contiter dataPtr = dataArray.cbegin();
				Array<bool>::const_contiter flagPtr = flagArray.cbegin();
			
				Image2D *real = &*result->_realImages[p];
				Image2D *imag = &*result->_imaginaryImages[p];
				Mask2D *mask = &*result->_flags[p];
				const size_t imgStride = real->Stride();
				const size_t mskStride = mask->Stride();
				num_t *realOutPtr = real->ValuePtr(curTimeIndex, 0);
				num_t *imagOutPtr = imag->ValuePtr(curTimeIndex, 0);
				bool *flagOutPtr = mask->ValuePtr(curTimeIndex, 0);
				
				for(size_t i=0;i!=p;++i) {
					++dataPtr;
					++flagPtr;
				}
				size_t frequencyCount = bandInfos[spw].channels.size();
				for(size_t ch=0;ch!=frequencyCount;++ch)
				{
					*realOutPtr = dataPtr->real();
					*imagOutPtr = dataPtr->imag();
					*flagOutPtr = *flagPtr;
					
					realOutPtr += imgStride;
					imagOutPtr += imgStride;
					flagOutPtr += mskStride;
					
					for(size_t i=0;i!=polarizationCount;++i) {
						++dataPtr;
						++flagPtr;
					}
				}
			}
		}
		
		// Store elements in matrix to the baseline map.
		for(size_t s=0; s!=sequenceCount; ++s)
		{
			for(size_t b=0; b!=bandCount; ++b)
			{
				size_t fbIndex = s*bandCount + b;
				for(size_t a1=0;a1!=antennaCount;++a1)
				{
					for(size_t a2=a1;a2!=antennaCount;++a2)
					{
						if(baselineCube[fbIndex][a1][a2] != 0)
						{
							BaselineID id;
							id.antenna1 = a1;
							id.antenna2 = a2;
							id.spw = b;
							id.sequenceId = s;
							_baselines.insert(std::pair<BaselineID, Result*>(id, baselineCube[fbIndex][a1][a2]));
						}
					}
				}
			}
		}
		_areFlagsChanged = false;
		_isRead = true;
		
		AOLogger::Debug << "Reading toke " << watch.ToString() << ".\n";
	}
}

void MemoryBaselineReader::PerformFlagWriteRequests()
{
	readSet();
	
	for(size_t i=0;i!=_writeRequests.size();++i)
	{
		const FlagWriteRequest &request = _writeRequests[i];
		BaselineID id;
		id.antenna1 = request.antenna1;
		id.antenna2 = request.antenna2;
		id.spw = request.spectralWindow;
		id.sequenceId = request.sequenceId;
		Result *result = _baselines[id];
		if(result->_flags.size() != request.flags.size())
			throw std::runtime_error("Polarizations do not match");
		for(size_t p=0;p!=result->_flags.size();++p)
			result->_flags[p] = Mask2D::CreateCopy(request.flags[p]);
	}
	_areFlagsChanged = true;
	
	_writeRequests.clear();
}

void MemoryBaselineReader::writeFlags()
{
	casacore::Table &table = *Table();
	
	ROScalarColumn<int>
		ant1Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA1)),
		ant2Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA2)),
		dataDescIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::DATA_DESC_ID)),
		fieldIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FIELD_ID));
	ROScalarColumn<double>
		timeColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::TIME));
	ArrayColumn<bool>
		flagColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FLAG));
	std::vector<size_t> dataIdToSpw;
	Set().GetDataDescToBandVector(dataIdToSpw);
	
	size_t polarizationCount = PolarizationCount();
		
	AOLogger::Debug << "Flags have changed, writing them back to the set...\n";
	
	double prevTime = -1.0;
	unsigned rowCount = table.nrow();
	size_t timeIndex = 0;
	size_t prevFieldId = size_t(-1), sequenceId = size_t(-1);
	for(unsigned rowIndex = 0;rowIndex < rowCount;++rowIndex)
	{
		size_t fieldId = fieldIdColumn(rowIndex);
		if(fieldId != prevFieldId)
		{
			prevFieldId = fieldId;
			sequenceId++;
			prevTime = -1.0;
		}
		double time = timeColumn(rowIndex);
		if(time != prevTime)
		{
			timeIndex = ObservationTimes(sequenceId).find(time)->second;
			prevTime = time;
		}
		
		size_t ant1 = ant1Column(rowIndex);
		size_t ant2 = ant2Column(rowIndex);
		size_t spw = dataIdToSpw[dataDescIdColumn(rowIndex)];
		if(ant1 > ant2) std::swap(ant1, ant2);
		
		size_t frequencyCount = Set().FrequencyCount(spw);
		IPosition flagShape = IPosition(2);
		flagShape[0] = polarizationCount;
		flagShape[1] = frequencyCount;
		casacore::Array<bool> flagArray(flagShape);
		
		BaselineID baselineID;
		baselineID.antenna1 = ant1;
		baselineID.antenna2 = ant2;
		baselineID.spw = spw;
		baselineID.sequenceId = sequenceId;
		std::map<BaselineID, Result*>::iterator resultIter = _baselines.find(baselineID);
		Result *result = resultIter->second;
		
		Array<bool>::contiter flagPtr = flagArray.cbegin();
		
		std::vector<Mask2D*> masks(polarizationCount);
		for(size_t p=0;p!=polarizationCount;++p)
			masks[p] = &*result->_flags[p];
		
		for(size_t ch=0;ch!=frequencyCount;++ch)
		{
			for(size_t p=0;p!=polarizationCount;++p)
			{
				*flagPtr = masks[p]->Value(timeIndex, ch);
				++flagPtr;
			}
		}
		
		flagColumn.put(rowIndex, flagArray);
	}
	
	_areFlagsChanged = false;
}

bool MemoryBaselineReader::IsEnoughMemoryAvailable(const std::string &filename)
{
	uint64_t size = MeasurementSetDataSize(filename);
	uint64_t totalMem = System::TotalMemory();
	
	if(size * 2 >= totalMem)
	{
		AOLogger::Warn
			<< (size/1000000) << " MB required, but " << (totalMem/1000000) << " MB available.\n"
			"Because this is not at least twice as much, direct read mode (slower!) will be used.\n";
		return false;
	} else {
		AOLogger::Debug
			<< (size/1000000) << " MB required, " << (totalMem/1000000)
			<< " MB available: will use memory read mode.\n";
		return true;
	}
}
