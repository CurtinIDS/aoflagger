#ifndef ANTENNAFLAGCOUNTPLOT_H
#define ANTENNAFLAGCOUNTPLOT_H

#include <string>
#include <map>

#include "../../structures/timefrequencymetadata.h"

class AntennaFlagCountPlot {
	public:
		AntennaFlagCountPlot() { }
		~AntennaFlagCountPlot() { }

		void Add(class TimeFrequencyData &data, TimeFrequencyMetaDataCPtr meta);
		void MakePlot();
		bool HasData() { return !_counts.empty(); }
		void Report();
	private:
		void WriteCounts();
		std::string formatPercentage(double percentage);

		struct MapItem {
			MapItem() : name(), autoCount(0), autoTotal(0), crossCount(0), crossTotal(0)
			{
			}
			MapItem(const MapItem &source) : name(source.name), autoCount(source.autoCount), autoTotal(source.autoTotal), crossCount(source.crossCount), crossTotal(source.crossTotal)
			{
			}
			MapItem &operator=(const MapItem &source)
			{
				name = source.name;
				autoCount = source.autoCount;
				autoTotal = source.autoTotal;
				crossCount = source.crossCount;
				crossTotal = source.crossTotal;
				return *this;
			}
			std::string name;
			long long autoCount, autoTotal, crossCount, crossTotal;
		};
		std::map<int, MapItem> _counts;
};

#endif
