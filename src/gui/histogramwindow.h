#ifndef GUI___HISTOGRAMWINDOW_H
#define GUI___HISTOGRAMWINDOW_H

#include "quality/histogrampage.h"

#include <gtkmm/window.h>

#include "../quality/histogramcollection.h"

class HistogramWindow : public Gtk::Window
{
	public:
		HistogramWindow(const HistogramCollection &histograms)
		{
			_histogramPage.SetHistograms(&histograms);
			add(_histogramPage);
			_histogramPage.show();
		}
		void SetStatistics(const HistogramCollection &histograms)
		{
			_histogramPage.SetHistograms(&histograms);
		}
	private:
		HistogramPage _histogramPage;
};

#endif
