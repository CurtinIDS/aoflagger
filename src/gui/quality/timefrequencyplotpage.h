#ifndef GUI_QUALITY__TIMEFREQUENCYPLOTPAGE_H
#define GUI_QUALITY__TIMEFREQUENCYPLOTPAGE_H

#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/frame.h>
#include <gtkmm/radiobutton.h>

#include "../imagewidget.h"

#include "../../quality/qualitytablesformatter.h"

#include "grayscaleplotpage.h"

class TimeFrequencyPlotPage : public GrayScalePlotPage {
	public:
		TimeFrequencyPlotPage();
    virtual ~TimeFrequencyPlotPage();
		
		sigc::signal<void, const std::string &> SignalStatusChange() { return _signalStatusChange; }
		
		void SetStatistics(class StatisticsCollection *statCollection)
		{
			_statCollection = statCollection;
			UpdateImage();
		}
		void CloseStatistics()
		{
			_statCollection = 0;
		}
		bool HasStatistics() const
		{
			return _statCollection != 0;
		}
		
	protected:
		virtual std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> ConstructImage(QualityTablesFormatter::StatisticKind kind);
	private:
		void onMouseMoved(size_t x, size_t y);
		
		class StatisticsCollection *_statCollection;
		
		sigc::signal<void, const std::string &> _signalStatusChange;
};

#endif
