#include "baselinereader.h"

#include <set>
#include <stdexcept>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>

#include <casacore/tables/DataMan/IncrStManAccessor.h>
#include <casacore/tables/DataMan/StandardStManAccessor.h>
#include <casacore/tables/DataMan/TiledStManAccessor.h>
#include <casacore/tables/Tables/TableIter.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include "timefrequencydata.h"

#include "../util/aologger.h"

BaselineReader::BaselineReader(const std::string &msFile)
	: _measurementSet(msFile), _table(0), _dataColumnName("DATA"), _subtractModel(false), _readData(true), _readFlags(true),
	_polarizationCount(0)
{
	try {
		_table = new casacore::MeasurementSet(_measurementSet.Path(), casacore::MeasurementSet::Update);
	} catch(std::exception &e)
	{
		AOLogger::Warn << "Read-write opening of file " << msFile << " failed, trying read-only...\n";
		_table = new casacore::MeasurementSet(_measurementSet.Path());
		AOLogger::Warn << "Table opened in read-only: writing not possible.\n";
	}
}

BaselineReader::~BaselineReader()
{
	delete _table;
}

void BaselineReader::initObservationTimes()
{
	if(_observationTimes.size() == 0)
	{
		AOLogger::Debug << "Initializing observation times...\n";
		size_t sequenceCount = _measurementSet.SequenceCount();
		_observationTimes.resize(sequenceCount);
		for(size_t sequenceId=0; sequenceId!=sequenceCount; ++sequenceId)
		{
			const std::set<double> &times = _measurementSet.GetObservationTimesSet(sequenceId);
			unsigned index = 0;
			for(std::set<double>::const_iterator i=times.begin();i!=times.end();++i)
			{
				_observationTimes[sequenceId].insert(std::pair<double,size_t>(*i, index));
				_observationTimesVector.push_back(*i);
				++index;
			}
		}
	}
}

void BaselineReader::AddReadRequest(size_t antenna1, size_t antenna2, size_t spectralWindow, size_t sequenceId)
{
	initObservationTimes();
	
	addReadRequest(antenna1, antenna2, spectralWindow, sequenceId, 0, _observationTimes[sequenceId].size());
}

TimeFrequencyData BaselineReader::GetNextResult(std::vector<class UVW> &uvw)
{
	size_t requestIndex = 0;
	TimeFrequencyData data;
	if(_polarizationCount == 4)
	{
		data = TimeFrequencyData(
			_results[requestIndex]._realImages[0], _results[requestIndex]._imaginaryImages[0],
			_results[requestIndex]._realImages[1], _results[requestIndex]._imaginaryImages[1],
			_results[requestIndex]._realImages[2], _results[requestIndex]._imaginaryImages[2],
			_results[requestIndex]._realImages[3], _results[requestIndex]._imaginaryImages[3]
			);
		data.SetIndividualPolarisationMasks(
			_results[requestIndex]._flags[0],
			_results[requestIndex]._flags[1],
			_results[requestIndex]._flags[2],
			_results[requestIndex]._flags[3]);
	} else if(_polarizationCount == 2)
	{
		data = TimeFrequencyData(AutoDipolePolarisation,
			_results[requestIndex]._realImages[0], _results[requestIndex]._imaginaryImages[0],
			_results[requestIndex]._realImages[1], _results[requestIndex]._imaginaryImages[1]);
		data.SetIndividualPolarisationMasks(
			_results[requestIndex]._flags[0],
			_results[requestIndex]._flags[1]);
	} else if(_polarizationCount == 1)
	{
		data = TimeFrequencyData(StokesIPolarisation,
			_results[requestIndex]._realImages[0], _results[requestIndex]._imaginaryImages[0]);
		data.SetGlobalMask(_results[requestIndex]._flags[0]);
	}
	uvw = _results[0]._uvw;
	
	_results.erase(_results.begin() + requestIndex);

	return data;
}

void BaselineReader::initializePolarizations()
{
	if(_polarizationCount == 0)
	{
		casacore::MeasurementSet ms(_measurementSet.Path());
		casacore::Table polTable = ms.polarization();
		casacore::ROArrayColumn<int> corTypeColumn(polTable, "CORR_TYPE"); 
		casacore::Array<int> corType = corTypeColumn(0);
		casacore::Array<int>::iterator iterend(corType.end());
		int polarizationCount = 0;
		for (casacore::Array<int>::iterator iter=corType.begin(); iter!=iterend; ++iter)
		{
			switch(*iter) {
				case 1: //_stokesIIndex = polarizationCount; break;
				case 5:
				case 9: //_xxIndex = polarizationCount; break;
				case 6:
				case 10:// _xyIndex = polarizationCount; break;
				case 7:
				case 11:// _yxIndex = polarizationCount; break;
				case 8:
				case 12: //_yyIndex = polarizationCount; break;
				break;
				default:
				{
					std::stringstream s;
					s << "There is a polarization in the measurement set that I can not handle (" << *iter << ", polarization index " << polarizationCount << ").";
					throw std::runtime_error(s.str());
				}
			}
			++polarizationCount;
		}
		_polarizationCount = polarizationCount;
	}
}

uint64_t BaselineReader::MeasurementSetDataSize(const string& filename)
{
	casacore::MeasurementSet ms(filename);
	
	casacore::MSSpectralWindow spwTable = ms.spectralWindow();
	
	casacore::ROScalarColumn<int> numChanCol(spwTable, casacore::MSSpectralWindow::columnName(casacore::MSSpectralWindowEnums::NUM_CHAN));
	size_t channelCount = numChanCol.get(0);
	if(channelCount == 0) throw std::runtime_error("No channels in set");
	if(ms.nrow() == 0) throw std::runtime_error("Table has no rows (no data)");
	
	typedef float num_t;
	typedef std::complex<num_t> complex_t;
	casacore::ROScalarColumn<int> ant1Column(ms, ms.columnName(casacore::MSMainEnums::ANTENNA1));
	casacore::ROScalarColumn<int> ant2Column(ms, ms.columnName(casacore::MSMainEnums::ANTENNA2));
	casacore::ROArrayColumn<complex_t> dataColumn(ms, ms.columnName(casacore::MSMainEnums::DATA));
	
	casacore::IPosition dataShape = dataColumn.shape(0);
	unsigned polarizationCount = dataShape[0];
	
	return
		(uint64_t) polarizationCount * (uint64_t) channelCount *
		(uint64_t) ms.nrow() * (uint64_t) (sizeof(num_t) * 2 + sizeof(bool));
}
