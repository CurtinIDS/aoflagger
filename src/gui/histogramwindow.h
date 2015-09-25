#ifndef GUI___HISTOGRAMWINDOW_H
#define GUI___HISTOGRAMWINDOW_H

#include "quality/histogrampage.h"

#include <gtkmm/window.h>

#include "../quality/histogramcollection.h"

class HistogramWindow : public Gtk::Window
{
	public:
		HistogramWindow(HistogramCollection &histograms)
		{
			_histogramPage.SetStatistics(histograms);
			add(_histogramPage);
			_histogramPage.show();
		}
		void SetStatistics(HistogramCollection &histograms)
		{
			_histogramPage.SetStatistics(histograms);
		}
	private:
		HistogramPage _histogramPage;
};

#endif
