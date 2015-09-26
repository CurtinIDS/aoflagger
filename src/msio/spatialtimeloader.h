#ifndef SPATIALTIMELOADER_H
#define SPATIALTIMELOADER_H

#include <cstring>

#include <casacore/tables/Tables/TableIter.h>

#include "../structures/timefrequencydata.h"
#include "../structures/measurementset.h"

/**
 * Loader for time x baseline matrices. These are mainly used for SVD experiments.
 * This class is used in the SpatialTimeImageSet .
 */
class SpatialTimeLoader
{
	public:
		explicit SpatialTimeLoader(MeasurementSet &measurementSet);
		~SpatialTimeLoader();

		TimeFrequencyData Load(unsigned channelIndex, bool fringeStop = true);

		unsigned ChannelCount() const { return _channelCount; }
		
		unsigned TimestepsCount() const { return _timestepsCount; }
	private:
		MeasurementSet &_measurementSet;
		casacore::Table *_sortedTable;
		casacore::TableIterator *_tableIter;
		unsigned _channelCount;
		unsigned _timestepsCount;
		unsigned _antennaCount;
		unsigned _polarizationCount;
};

#endif
