#ifndef FREQUENCYFLAGCOUNTPLOT_H
#define FREQUENCYFLAGCOUNTPLOT_H

#include <map>

#include "../../msio/timefrequencymetadata.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class FrequencyFlagCountPlot{
	public:
		FrequencyFlagCountPlot() : _ignoreFirstChannel(true) { }
		~FrequencyFlagCountPlot() { }

		void Add(class TimeFrequencyData &data, TimeFrequencyMetaDataCPtr meta);
		void WriteCounts();
		bool HasData() { return !_counts.empty(); }
		void MakePlot();
		void Report();
	private:
		struct MapItem {
			MapItem() : count(0), total(0) { }
			long long count, total;
		};
		std::string formatPercentage(double percentage);
		std::string formatFrequency(double frequencyHz);
		void formatToThreeDigits(std::stringstream &stream, int number)
		{
			if(number < 100) stream << '0';
			if(number < 10) stream << '0';
			stream << number;
		}
		std::string formatIndex(int index)
		{
			std::stringstream s;
			if(index < 100) s << ' ';
			if(index < 10) s << ' ';
			s << index;
			return s.str();
		}
		// In lofar, the first channel of every subband is flagged, because it overlaps with
		// the previous subband. 
		bool _ignoreFirstChannel;

		std::map<double, struct MapItem> _counts;
};

#endif
