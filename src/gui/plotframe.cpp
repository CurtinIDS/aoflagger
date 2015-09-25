#include <limits>

#include "plot/plot2d.h"

#include "plotframe.h"

PlotFrame::PlotFrame() : _plotData(0), _selectedXStart(0), _selectedYStart(0), _selectedXEnd(0), _selectedYEnd(0)
{
	pack_start(_plot);
	_plot.show();
}

PlotFrame::~PlotFrame()
{
	if(_plotData != 0)
		delete _plotData;
}

void PlotFrame::plot()
{
	_plot.Clear();
	if(_plotData != 0)
		delete _plotData;
	_plotData = new Plot2D();

	bool drawn = false;
	if(_data.HasXX()) {
		plotTimeGraph(_data, "XX", XXPolarisation);
		drawn = true;
	}
	if(_data.HasXY()) {
		plotTimeGraph(_data, "XY", XYPolarisation);
		drawn = true;
	}
	if(_data.HasYX()) {
		plotTimeGraph(_data, "YX", YXPolarisation);
		drawn = true;
	}
	if(_data.HasYY()) {
		plotTimeGraph(_data, "YY", YYPolarisation);
		drawn = true;
	}

	if(_data.Polarisation() == StokesIPolarisation)
	{
		plotTimeGraph(_data, "Stokes I");
		drawn = true;
	} else if(!drawn) {
		plotTimeGraph(_data, "Data");
	}

	_plot.SetPlot(*_plotData);
}

void PlotFrame::plotTimeGraph(const TimeFrequencyData &data, const std::string &label, enum PolarisationType polarisation)
{
	TimeFrequencyData *convertedData = data.CreateTFData(polarisation);
	plotTimeGraph(*convertedData, label);
	delete convertedData;
}

void PlotFrame::plotTimeGraph(const TimeFrequencyData &data, const std::string &label)
{
	_plotData->StartLine(label);
	Image2DCPtr image = data.GetSingleImage();
	Mask2DCPtr mask = data.GetSingleMask();

	for(size_t x=0;x<image->Width();++x)
	{
		size_t count = 0;
		num_t value = 0.0;

		for(size_t y=_selectedYStart;y<_selectedYEnd;++y)
		{
			if(!mask->Value(x, y))
			{
				++count;
				value += image->Value(x, y);
			}
		}
		if(count > 0)
			_plotData->PushDataPoint(x, value / (num_t) count);
		else
			_plotData->PushDataPoint(x, std::numeric_limits<num_t>::quiet_NaN());
	}
}
