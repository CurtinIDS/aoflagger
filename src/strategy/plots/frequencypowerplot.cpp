#include "frequencypowerplot.h"

#include <fstream>

#include "../../util/plot.h"

#include "../../structures/timefrequencydata.h"
#include "../../structures/timefrequencymetadata.h"

FrequencyPowerPlot::~FrequencyPowerPlot()
{
	if(_plot != 0) delete _plot;
}

void FrequencyPowerPlot::Add(class TimeFrequencyData &data, TimeFrequencyMetaDataCPtr meta)
{
	Image2DCPtr image = data.GetSingleImage();
	Mask2DCPtr mask = data.GetSingleMask();
	for(size_t y=0;y<image->Height();++y)
	{
		double frequency = meta->Band().channels[y].frequencyHz;
		size_t count = 0;
		long double value = 0.0L;

		for(size_t x=0;x<image->Width();++x)
		{
			if(!mask->Value(x, y))
			{
				++count;
				value += image->Value(x, y);
			}
		}
		MapItem &item = _values[frequency];
		item.total += value;
		item.count += count;
	}
} 

void FrequencyPowerPlot::WriteToText()
{
	std::ofstream file("frequency-vs-counts.txt");
	for(std::map<double, struct MapItem>::const_iterator i=_values.begin();i!=_values.end();++i)
	{
		file << i->first << "\t" << i->second.total << "\t" << i->second.count << "\n";
	}
	file.close();
}

void FrequencyPowerPlot::MakePlot()
{
	if(_plot == 0)
	{
		Plot plot("frequency-vs-power.pdf");
		plot.SetXAxisText("Frequency (MHz)");
		plot.SetYAxisText("Vis. power");
		plot.SetLogScale(false, _logYAxis);
		plot.StartScatter();
		AddCurrentLine(plot);
		plot.Close();
		plot.Show();
	} else {
		_plot->Close();
		_plot->Show();
	}
}

void FrequencyPowerPlot::AddCurrentLine(class Plot &plot)
{
	for(std::map<double, struct MapItem>::const_iterator i=_values.begin();i!=_values.end();++i)
	{
		plot.PushDataPoint(i->first / 1000000.0L, (long double) i->second.total / (long double) i->second.count);
	}
}

void FrequencyPowerPlot::StartNewLine(const std::string &lineTitle)
{
	if(_plot == 0)
	{
		_plot = new Plot("frequency-vs-power.pdf");
		_plot->SetXAxisText("Frequency (MHz)");
		_plot->SetYAxisText("Vis. power");
		_plot->SetFontSize(12);
		_plot->SetLogScale(false, _logYAxis);
		_plot->StartLine(lineTitle);
	} else {
		AddCurrentLine(*_plot);
		_values.clear();
		_plot->StartLine(lineTitle);
	}
}
