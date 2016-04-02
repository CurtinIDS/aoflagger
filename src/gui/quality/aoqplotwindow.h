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

#include "antennaeplotpage.h"
#include "baselineplotpage.h"
#include "blengthplotpage.h"
#include "frequencyplotpage.h"
#include "histogrampage.h"
#include "openoptionswindow.h"
#include "summarypage.h"
#include "timefrequencyplotpage.h"
#include "timeplotpage.h"

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
		void onSwitchPage(Gtk::Widget *page, guint pageNr)
		{
			switch(pageNr)
			{
				case 0: SetStatus("Baseline statistics"); break;
				case 1: SetStatus("Antennae statistics"); break;
				case 2: SetStatus("Baseline length statistics");  break;
				case 3: SetStatus("Time statistics"); break;
				case 4: SetStatus("Frequency statistics"); break;
				case 5: SetStatus("Time-frequency statistics");  break;
				case 6: SetStatus("Summary"); break;
			}
		}
		
		void onChangeSheet();
		void showBaselineSheet();
		void showAntennaeSheet();
		void showBaselineLengthSheet();
		void showTimeSheet();
		void showFrequencySheet();
		void showTimeFrequencySheet();
		void showSummarySheet();
		void showHistogramSheet();
		
		void setShowHistograms(bool show)
		{
			//_histogramPage.set_visible(show);
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
		Gtk::Notebook _notebook;
		Gtk::Statusbar _statusBar;
		
		//BaselinePlotPage _baselinePlotPage;
		//AntennaePlotPage _antennaePlotPage;
		//BLengthPlotPage  _bLengthPlotPage;
		//TimeFrequencyPlotPage _timeFrequencyPlotPage;
		//TimePlotPage _timePlotPage;
		//FrequencyPlotPage _frequencyPlotPage;
		//SummaryPage _summaryPage;
		//HistogramPage _histogramPage;
		std::unique_ptr<Gtk::HBox> _activeSheet;
		
		OpenOptionsWindow _openOptionsWindow;

		bool _isOpen;
		class StatisticsCollection *_statCollection;
		class HistogramCollection *_histCollection;
		class StatisticsCollection *_fullStats;
		std::vector<class AntennaInfo> _antennas;
		size_t _polarizationCount;
};

#endif
