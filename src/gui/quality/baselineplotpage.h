#ifndef GUI_QUALITY__BASELINEPLOTPAGE_H
#define GUI_QUALITY__BASELINEPLOTPAGE_H

#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/frame.h>
#include <gtkmm/radiobutton.h>

#include "../imagewidget.h"

#include "../../quality/qualitytablesformatter.h"

#include "grayscaleplotpage.h"

class BaselinePlotPage : public GrayScalePlotPage {
	public:
		BaselinePlotPage();
    virtual ~BaselinePlotPage();
		
		sigc::signal<void, const std::string &> SignalStatusChange() { return _signalStatusChange; }
		
		virtual void SetStatistics(const StatisticsCollection* statCollection, const std::vector<class AntennaInfo>& antennas) override final
		{
			_statCollection = statCollection;
			_antennas = &antennas;
			updateImage();
		}
		void CloseStatistics()
		{
			_statCollection = 0;
			_antennas = 0;
		}
		bool HasStatistics() const
		{
			return _statCollection != 0;
		}
	protected:
		virtual std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> constructImage(QualityTablesFormatter::StatisticKind kind) override final;
	private:
		void onMouseMoved(size_t x, size_t y);
		
		const StatisticsCollection *_statCollection;
		const std::vector<class AntennaInfo> *_antennas;
		
		sigc::signal<void, const std::string &> _signalStatusChange;
};

#endif
