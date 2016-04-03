#ifndef AOQPLOT_WINDOW_H
#define AOQPLOT_WINDOW_H

#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/menu.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/radiomenuitem.h>
#include <gtkmm/radiotoolbutton.h>
#include <gtkmm/window.h>

#include "../imagewidget.h"

#include "../../quality/qualitytablesformatter.h"

#include "plotsheet.h"
#include "openoptionswindow.h"

#include "../../structures/antennainfo.h"

class AOQPlotWindow : public Gtk::Window {
	public:
		AOQPlotWindow();
    virtual ~AOQPlotWindow()
    { 
			close();
		}
    
		void Open(const std::vector<std::string>& files);
		void Open(const std::string& file)
		{
			std::vector<std::string> files(1);
			files[0] = file;
			Open(files);
		}
		void OpenWithoutGUI(const std::vector<std::string>& files)
		{
			onOpenOptionsSelected(files, true, true, 1000, 1000, false);
		}
		void SetStatus(const std::string &newStatus)
		{
			onStatusChange(newStatus);
		}
		struct PlotSavingData
		{
			QualityTablesFormatter::StatisticKind statisticKind;
			std::string filenamePrefix;
		};
		void Save(const PlotSavingData& data);
	private:
		void onOpenOptionsSelected(const std::vector<std::string>& files, bool downsampleTime, bool downsampleFreq, size_t timeSize, size_t freqSize, bool correctHistograms);
		void close();
		void readStatistics(const std::vector<std::string>& files, bool downsampleTime, bool downsampleFreq, size_t timeSize, size_t freqSize, bool correctHistograms);
		void readDistributedObservation(const std::string& filename, bool correctHistograms);
		void readMetaInfoFromMS(const std::string& filename);
		void readAndCombine(const std::string& filename);
		
		void onHide()
		{
			//Gtk::Application::quit();
		}
		void onStatusChange(const std::string &newStatus);
		
		void onChangeSheet();
		
		void setShowHistograms(bool show)
		{
			_histogramMI.set_sensitive(show);
		}
		
		int _activeSheetIndex;
		Gtk::Toolbar _toolbar;
		Gtk::MenuToolButton _pageMenuButton;
		Gtk::Menu _pageMenu;
		Gtk::RadioButtonGroup _pageGroup, _statisticsGroup;
		Gtk::RadioMenuItem _baselineMI, _antennaeMI, _bLengthMI, _timeMI, _frequencyMI, _timeFrequencyMI, _summaryMI, _histogramMI;
		Gtk::RadioToolButton _countButton, _meanButton, _stddevButton;
		
		Gtk::VBox _vBox;
		Gtk::Statusbar _statusBar;
		
		std::unique_ptr<PlotSheet> _activeSheet;
		
		OpenOptionsWindow _openOptionsWindow;

		bool _isOpen;
		class StatisticsCollection *_statCollection;
		class HistogramCollection *_histCollection;
		class StatisticsCollection *_fullStats;
		std::vector<class AntennaInfo> _antennas;
		size_t _polarizationCount;
};

#endif
