#include "antennaflagcountplot.h"

#include <fstream>
#include <iomanip>

#include "../../util/aologger.h"
#include "../../util/plot.h"

#include "../../structures/timefrequencydata.h"
#include "../../structures/timefrequencymetadata.h"

void AntennaFlagCountPlot::Add(class TimeFrequencyData &data, TimeFrequencyMetaDataCPtr meta)
{
	for(size_t maskIndex=0;maskIndex<data.MaskCount();++maskIndex)
	{
		Mask2DCPtr mask = data.GetMask(maskIndex);
		long long count = 0;

		for(size_t y=0;y<mask->Height();++y)
		{
			for(size_t x=0;x<mask->Width();++x)
			{
				if(mask->Value(x, y))
					++count;
			}
		}

		long long total = (long long) mask->Width() * (long long) mask->Height();

		MapItem item1;
		if(_counts.count(meta->Antenna1().id)==0)
			item1.name = meta->Antenna1().name;
		else
			item1 = _counts[meta->Antenna1().id];

		if(meta->Antenna1().id == meta->Antenna2().id)
		{
			item1.autoTotal += total;
			item1.autoCount += count;
			_counts[meta->Antenna1().id] = item1;
		} else {
			MapItem item2;
			if(_counts.count(meta->Antenna2().id)==0)
				item2.name = meta->Antenna2().name;
			else
				item2 = _counts[meta->Antenna2().id];

			item1.crossTotal += total;
			item1.crossCount += count;
			item2.crossTotal += total;
			item2.crossCount += count;
			_counts[meta->Antenna1().id] = item1;
			_counts[meta->Antenna2().id] = item2;
		}
	}
} 

void AntennaFlagCountPlot::MakePlot()
{
	Plot plot("antenna-flag-counts.pdf");
	plot.SetXAxisText("Antenna (index)");
	plot.SetYAxisText("Flagged (%)");
	plot.StartBoxes("Auto-correlation");
	plot.SetXRangeAutoMax(0);
	plot.SetYRangeAutoMax(0);
	for(std::map<int, MapItem>::const_iterator i=_counts.begin();i!=_counts.end();++i)
	{
		plot.PushDataPoint(i->first, 100.0L * (long double) i->second.autoCount / (long double) i->second.autoTotal);
	}
	plot.StartBoxes("Cross-correlation");
	for(std::map<int, MapItem>::const_iterator i=_counts.begin();i!=_counts.end();++i)
	{
		plot.PushDataPoint((long double) i->first + 0.3, 100.0L * (long double) i->second.crossCount / (long double) i->second.crossTotal);
	}
	plot.Close();
	plot.Show();
}

void AntennaFlagCountPlot::WriteCounts()
{
	std::ofstream file("antenna-vs-counts.txt");
	file << std::setprecision(14);
	for(std::map<int, MapItem>::const_iterator i=_counts.begin();i!=_counts.end();++i)
	{
		file << i->second.name << "\t" << (100.0L * (long double) i->second.autoCount / (long double) i->second.autoTotal) << "\t" << (100.0L * (long double) i->second.crossCount / (long double) i->second.crossTotal) << "\n";
	}
	file.close();
}

void AntennaFlagCountPlot::Report()
{
	for(std::map<int, MapItem>::const_iterator i=_counts.begin();i!=_counts.end();++i)
	{
		if(i->second.autoTotal != 0)
		{
			AOLogger::Info
				<< "Flagged in autocorrelations of antenna "
				<< i->second.name << ": "
				<< formatPercentage(100.0L * (long double) i->second.autoCount / (long double) i->second.autoTotal)
				<< "%\n";
		}
		if(i->second.crossTotal != 0)
		{
			AOLogger::Info
				<< "Flagged in cross correlations with antenna "
				<< i->second.name << ": "
				<< formatPercentage(100.0L * (long double) i->second.crossCount / (long double) i->second.crossTotal)
				<< "%\n";
		}
	}
}

std::string AntennaFlagCountPlot::formatPercentage(double percentage)
{
	std::stringstream s;
	if(percentage >= 1.0)
		s << round(percentage*10.0)/10.0;
	else if(percentage >= 0.1)
		s << round(percentage*100.0)/100.0;
	else
		s << round(percentage*1000.0)/1000.0;
	return s.str();
}
