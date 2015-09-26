#ifndef ITERATIONSPLOT_H
#define ITERATIONSPLOT_H

#include <string>
#include <map>

#include "../../structures/mask2d.h"
#include "../../structures/timefrequencymetadata.h"

class IterationsPlot
{
public:
	IterationsPlot() { }
	~IterationsPlot() { }

	void Add(class TimeFrequencyData &data, TimeFrequencyMetaDataCPtr meta );
	void MakePlot();
	bool HasData() { return !_stats.empty(); }
private:
	struct Item
	{
		Item() : flaggedRatio(0.0), mode(0.0), winsorizedMode(0.0)
		{
		}
		double flaggedRatio;
		num_t mode, winsorizedMode;
	};
	std::vector<Item> _stats;
};

#endif
