#include "iterationsplot.h"

#include "../../util/aologger.h"
#include "../../util/plot.h"

#include "../../structures/timefrequencydata.h"
#include "../../structures/timefrequencymetadata.h"

#include "../algorithms/thresholdtools.h"

void IterationsPlot::Add(TimeFrequencyData &data, TimeFrequencyMetaDataCPtr)
{
	Item item;
	Mask2DCPtr mask = data.GetSingleMask();
	item.mode = ThresholdTools::Mode(data.GetSingleImage(), mask);
	item.winsorizedMode = ThresholdTools::WinsorizedMode(data.GetSingleImage(), mask);
	item.flaggedRatio = (double) mask->GetCount<true>() / ((double) mask->Width() * (double) mask->Height());
	_stats.push_back(item);
}

void IterationsPlot::MakePlot()
{
	Plot plotA("iterations-flags.pdf");
	plotA.SetXAxisText("Iteration number");
	plotA.SetYAxisText("Flagged (%)");
	plotA.StartLine();
	plotA.SetXRange(1, _stats.size());
	for(unsigned i=0;i<_stats.size();++i)
	{
		plotA.PushDataPoint(i+1, _stats[i].flaggedRatio * 100.0);
	}
	plotA.Close();
	plotA.Show();

	Plot plotB("iterations-modes.pdf");
	plotB.SetXAxisText("Iteration number");
	plotB.SetYAxisText("Mode");
	plotB.StartLine("Normal mode");
	plotB.SetXRange(1, _stats.size());
	for(unsigned i=0;i<_stats.size();++i)
	{
		plotB.PushDataPoint(i+1, _stats[i].mode);
	}
	plotB.StartLine("Winsorized mode");
	for(unsigned i=0;i<_stats.size();++i)
	{
		plotB.PushDataPoint(i+1, _stats[i].winsorizedMode);
	}
	plotB.Close();
	plotB.Show();
}
