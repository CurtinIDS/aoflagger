#ifndef MEASUREMENTSET_H
#define MEASUREMENTSET_H

#include <string>
#include <vector>
#include <utility>
#include <set>

#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <casacore/tables/DataMan/DataManager.h>

#include "../strategy/control/types.h"

#include "antennainfo.h"

class MSIterator {
	public:
		MSIterator(class MeasurementSet &ms, bool hasCorrectedData=true);
		~MSIterator();
		MSIterator &operator++() { _row++; return *this; }
		//size_t TotalRows() { return _table->nrow(); }

		casacore::Complex Data(unsigned frequencyIndex, unsigned polarisation)
		{
			return (*_dataCol)(_row)(casacore::IPosition(2, frequencyIndex, polarisation));
		}

		bool Flag(unsigned frequencyIndex, unsigned polarisation) { return (*_flagCol)(_row)(casacore::IPosition(2, frequencyIndex, polarisation)); }
		casacore::Array<bool>::const_iterator FlagIterator()
		{
			return (*_flagCol)(_row).begin();
		}

		casacore::Complex CorrectedData(unsigned frequencyIndex, unsigned polarisation)
		{
			return (*_correctedDataCol)(_row)(casacore::IPosition(2, frequencyIndex, polarisation));
		}

		casacore::Array<casacore::Complex>::const_iterator CorrectedDataIterator()
		{
			return (*_correctedDataCol)(_row).begin();
		}

		int Field() { return (*_fieldCol)(_row); }
		double Time() { return (*_timeCol)(_row); }
		unsigned Antenna1() { return (*_antenna1Col)(_row); }
		unsigned Antenna2() { return (*_antenna2Col)(_row); }
		unsigned ScanNumber() { return (*_scanNumberCol)(_row); }
		class UVW UVW() {
			class UVW uvw;
			casacore::Array<double> arr = (*_uvwCol)(_row);
			casacore::Array<double>::const_iterator i = arr.begin();
			uvw.u = *i; ++i;
			uvw.v = *i; ++i;
			uvw.w = *i;
			return uvw;
		}
		unsigned Window() { return (*_windowCol)(_row); }
	private:
		unsigned long _row;
		casacore::ROScalarColumn<int> *_antenna1Col;
		casacore::ROScalarColumn<int> *_antenna2Col;
		casacore::ROArrayColumn<casacore::Complex> *_dataCol;
		casacore::ROArrayColumn<bool> *_flagCol;
		casacore::ROArrayColumn<casacore::Complex> *_correctedDataCol;
		casacore::ROScalarColumn<double> *_timeCol;
		casacore::ROScalarColumn<int> *_fieldCol;
		casacore::ROScalarColumn<int> *_scanNumberCol;
		casacore::ROArrayColumn<double> *_uvwCol;
		casacore::ROScalarColumn<int> *_windowCol;
		casacore::MeasurementSet *_table;
};

class MeasurementSet {
	public:
		class Sequence;
		
		MeasurementSet(const std::string &path) throw()
			: _path(path), _isMainTableDataInitialized(false)
		{
			initializeOtherData();
		}
		
		~MeasurementSet();
		
		size_t FrequencyCount(size_t bandIndex)
		{
			return _bands[bandIndex].channels.size();
		}
		
		size_t TimestepCount()
		{
			initializeMainTableData();
			return _observationTimes.size();
		}
		
		size_t TimestepCount(size_t sequenceId)
		{
			initializeMainTableData();
			return _observationTimesPerSequence[sequenceId].size();
		}
		
		size_t PolarizationCount();
		
		static size_t PolarizationCount(const std::string &filename);
		
		size_t AntennaCount() const
		{
			return _antennas.size();
		}
		
		size_t BandCount() const
		{ 
			return _bands.size();
		}
		
		size_t FieldCount() const
		{
			return _fields.size();
		}
		
		size_t RowCount() const
		{
			return _rowCount;
		}
		
		static size_t BandCount(const std::string &filename);
		
		/**
		 * Get number of sequences. A sequence is a contiguous number of scans
		 * on the same field. Thus, the next sequence starts as soon as the
		 * fieldId changes (possibly to a previous field)
		 */
		size_t SequenceCount()
		{
			initializeMainTableData();
			return _observationTimesPerSequence.size();
		}
		
		const AntennaInfo &GetAntennaInfo(unsigned antennaId) const
		{
			return _antennas[antennaId];
		}
		
		//static BandInfo GetBandInfo(const std::string &filename, unsigned bandIndex);
		
		const BandInfo &GetBandInfo(unsigned bandIndex) const
		{
			return _bands[bandIndex];
		}
		
		const FieldInfo &GetFieldInfo(unsigned fieldIndex) const
		{
			return _fields[fieldIndex];
		}
		
		void GetDataDescToBandVector(std::vector<size_t>& dataDescToBand);
		
		std::string Path() const { return _path; }
		
		void GetBaselines(std::vector<std::pair<size_t,size_t> > &baselines)
		{
			initializeMainTableData();
			baselines = _baselines;
		}
		
		const std::vector<Sequence> &GetSequences()
		{
			initializeMainTableData();
			return _sequences;
		}
		
		const std::set<double> &GetObservationTimesSet()
		{
			initializeMainTableData();
			return _observationTimes;
		}
		const std::set<double> &GetObservationTimesSet(size_t sequenceId)
		{
			initializeMainTableData();
			return _observationTimesPerSequence[sequenceId];
		}
		
		std::vector<double> *CreateObservationTimesVector()
		{
			initializeMainTableData();
			std::vector<double> *times = new std::vector<double>();
			for(std::set<double>::const_iterator i=_observationTimes.begin();i!=_observationTimes.end();++i)
				times->push_back(*i);
			return times;
		}
		
		bool HasRFIConsoleHistory();
		
		void GetAOFlaggerHistory(std::ostream &stream);
		
		void AddAOFlaggerHistory(const class rfiStrategy::Strategy &strategy, const std::string &commandline);
		
		std::string GetStationName() const;
		
		bool IsChannelZeroRubish();
		
		const std::string &TelescopeName() const { return _telescopeName; }
		
		class Sequence
		{
			public:
				Sequence(unsigned _antenna1, unsigned _antenna2, unsigned _spw, unsigned _sequenceId, unsigned _fieldId) :
					antenna1(_antenna1), antenna2(_antenna2),
					spw(_spw), sequenceId(_sequenceId),
					fieldId(_fieldId)
					{ }
				unsigned antenna1, antenna2;
				unsigned spw;
				unsigned sequenceId;
				unsigned fieldId;
				
				bool operator<(const Sequence &rhs) const
				{
					if(antenna1 < rhs.antenna1) return true;
					else if(antenna1 == rhs.antenna1)
					{
						if(antenna2 < rhs.antenna2) return true;
						else if(antenna2 == rhs.antenna2)
						{
							if(spw < rhs.spw) return true;
							else if(spw == rhs.spw)
							{
								return sequenceId < rhs.sequenceId;
							}
						}
					}
					return false;
				}
				bool operator==(const Sequence &rhs) const
				{
					return antenna1==rhs.antenna1 && antenna2==rhs.antenna2 &&
						spw==rhs.spw && sequenceId==rhs.sequenceId;
				}
		};
	private:
		void initializeMainTableData();
		
		void initializeOtherData();
		
		void initializeAntennas(casacore::MeasurementSet &ms);
		void initializeBands(casacore::MeasurementSet &ms);
		void initializeFields(casacore::MeasurementSet &ms);
		void initializeObservation(casacore::MeasurementSet &ms);

		const std::string _path;
		
		size_t _rowCount;
		
		bool _isMainTableDataInitialized;
		
		std::vector<std::pair<size_t,size_t> > _baselines;
		
		std::set<double> _observationTimes;
		
		std::vector<std::set<double> > _observationTimesPerSequence;

		std::vector<AntennaInfo> _antennas;
		
		std::vector<BandInfo> _bands;
		
		std::vector<FieldInfo> _fields;
		
		std::vector<Sequence> _sequences;
		
		std::string _telescopeName;
};

#endif
