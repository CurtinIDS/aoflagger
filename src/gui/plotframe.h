#ifndef PLOTFRAME_H
#define PLOTFRAME_H

#include <gtkmm/box.h>

#include "../msio/timefrequencydata.h"

#include "plot/plotwidget.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class PlotFrame : public Gtk::HBox {
	public:
		PlotFrame();
		~PlotFrame();

		void SetTimeFrequencyData(const TimeFrequencyData &data)
		{
			_data = data;
		}
		void SetSelectedSample(size_t x, size_t y)
		{
			_selectedXStart = x;
			_selectedYStart = y;
			_selectedXEnd = x+1;
			_selectedYEnd = y+1;
		}
		void Update() { plot(); }
	private:
		TimeFrequencyData _data;
		PlotWidget _plot;
		class Plot2D *_plotData;

		size_t _selectedXStart, _selectedYStart;
		size_t _selectedXEnd, _selectedYEnd;

		void plot();
		void plotTimeGraph(const TimeFrequencyData &data, const std::string &label, enum PolarisationType polarisation);
		void plotTimeGraph(const TimeFrequencyData &data, const std::string &label);
};

#endif
