#ifndef FRINGETESTCREATER_H
#define FRINGETESTCREATER_H

#include "../../msio/timefrequencymetadata.h"

class FringeTestCreater {
	public:
		static void AddStaticFringe(class TimeFrequencyData &ftData, TimeFrequencyMetaDataCPtr metaData, long double strength);
	private:
		FringeTestCreater() { }
		~FringeTestCreater() { }
};

#endif
