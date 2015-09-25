#ifndef TIMEFLAGCOUNTPLOT_H
#define TIMEFLAGCOUNTPLOT_H

#include <map>

#include "../../msio/timefrequencymetadata.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class TimeFlagCountPlot {
	public:
		TimeFlagCountPlot() : _startTime(0) { };
		~TimeFlagCountPlot() { }

		void Add(class TimeFrequencyData &data, TimeFrequencyMetaDataCPtr meta);
		bool HasData() { return !_counts.empty(); }
		void MakePlot();
	private:
		void WriteCounts();
		struct MapItem {
			MapItem() : count(0), total(0) { }
			long long count, total;
		};

		std::map<double, struct MapItem> _counts;
		double _startTime;
};

#endif // TIMEFLAGCOUNTPLOT_H
