#ifndef PLOT_SHEET_H
#define PLOT_SHEET_H

#include "../../structures/antennainfo.h"

#include <gtkmm/box.h>
#include <gtkmm/toolbar.h>

#include <vector>

class PlotSheet : public Gtk::HBox
{
public:
	virtual void SetStatistics(const class StatisticsCollection* statCollection, const std::vector<class AntennaInfo>& antennas) { }
	
	virtual void SetHistograms(const class HistogramCollection* histograms) { }
	
	virtual void CloseStatistics() = 0;

	sigc::signal<void, const std::string &> SignalStatusChange() { return _signalStatusChange; }

	virtual void InitializeToolbar(Gtk::Toolbar& toolbar) { };
	
protected:
	sigc::signal<void, const std::string &> _signalStatusChange;
};

#endif
