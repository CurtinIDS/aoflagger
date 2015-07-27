#ifndef BASELINEMATRIXLOADER_H
#define BASELINEMATRIXLOADER_H

#include <cstring>

#include <casacore/tables/Tables/TableIter.h>

#include "timefrequencydata.h"
#include "measurementset.h"

class BaselineMatrixLoader
{
	public:
		explicit BaselineMatrixLoader(MeasurementSet &measurementSet);
		~BaselineMatrixLoader();

		TimeFrequencyData Load(size_t timeIndex)
		{
			return LoadSummed(timeIndex);
		}
		void LoadPerChannel(size_t timeIndex, std::vector<TimeFrequencyData> &data);

		size_t TimeIndexCount() const { return _timeIndexCount; }
		class SpatialMatrixMetaData &MetaData() const
		{
			return *_metaData;
		}
		size_t FrequencyCount() const { return _frequencyCount; }
	private:
		TimeFrequencyData LoadSummed(size_t timeIndex);

		casacore::Table *_sortedTable;
		casacore::TableIterator *_tableIter;
		size_t _currentIterIndex;
		MeasurementSet _measurementSet;
		size_t _timeIndexCount;
		class SpatialMatrixMetaData *_metaData;
		size_t _frequencyCount;
};

#endif
