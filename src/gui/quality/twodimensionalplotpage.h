#ifndef GUI_QUALITY__2DPLOTPAGE_H
#define GUI_QUALITY__2DPLOTPAGE_H

#include <gtkmm/toggletoolbutton.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/separatortoolitem.h>

#include "../../quality/qualitytablesformatter.h"

#include "../plot/plot2d.h"
#include "../plot/plotwidget.h"

#include "plotsheet.h"

#include <set>

class TwoDimensionalPlotPage : public PlotSheet {
	public:
		TwoDimensionalPlotPage();
    virtual ~TwoDimensionalPlotPage();

		virtual void SetStatistics(const StatisticsCollection* statCollection, const std::vector<class AntennaInfo>& antennas) override final
		{
			processStatistics(statCollection, antennas);
			
			_statCollection = statCollection;
			updatePlot();
		}
		
		virtual void CloseStatistics() override final
		{
			_statCollection = 0;
		}
		
		virtual void InitializeToolbar(Gtk::Toolbar& toolbar) override final;
		
		bool HasStatistics() const
		{
			return _statCollection != 0;
		}
		
		void SavePdf(const std::string& filename, QualityTablesFormatter::StatisticKind kind);
	protected:
		virtual void processStatistics(const StatisticsCollection *, const std::vector<AntennaInfo> &)
		{
		}
		
		virtual const std::map<double, class DefaultStatistics> &getStatistics() const = 0;
		
		virtual void startLine(Plot2D &plot, const std::string &name, const std::string &yAxisDesc) = 0;
		
		virtual void processPlot(Plot2D &plot)
		{
		}
		
		virtual void addCustomPlotButtons(Gtk::Toolbar &container)
		{
		}
		
		const StatisticsCollection *getStatCollection() const
		{
			return _statCollection;
		}
		void updatePlot();
	private:
		enum PhaseType { AmplitudePhaseType, PhasePhaseType, RealPhaseType, ImaginaryPhaseType} ;
		
		void updatePlotForSettings(
			const std::set<QualityTablesFormatter::StatisticKind>& kinds,
			const std::set<std::pair<unsigned int, unsigned int> >& pols,
			const std::set<PhaseType>& phases
		);
		
		void updatePlotConfig();
		void updateDataWindow();
		
		inline double getValue(enum PhaseType Phase, const std::complex<long double> val);
		
		std::set<QualityTablesFormatter::StatisticKind> getSelectedKinds() const;
		std::set<std::pair<unsigned, unsigned> > getSelectedPolarizations() const;
		std::set<enum PhaseType> getSelectedPhases() const;
		
		void plotStatistic(QualityTablesFormatter::StatisticKind kind, unsigned polA, unsigned polB, PhaseType phase, const std::string& yDesc);
		
		void initStatisticKindButtons(Gtk::Toolbar& toolbar);
		void initPolarizationButtons(Gtk::Toolbar& toolbar);
		void initPhaseButtons(Gtk::Toolbar& toolbar);
		void initPlotButtons(Gtk::Toolbar& toolbar);
		
		void onLogarithmicClicked()
		{
			_zeroAxisButton.set_sensitive(!_logarithmicButton.get_active());
			updatePlotConfig();
		}
		void onPlotPropertiesClicked();
		void onDataExportClicked();
		
		Gtk::SeparatorToolItem _separator1, _separator2, _separator3, _separator4;
		
		Gtk::ToggleToolButton _countButton, _meanButton, _stdDevButton, _varianceButton, _dCountButton, _dMeanButton, _dStdDevButton,  _rfiPercentageButton;
		
		Gtk::ToggleToolButton _polXXButton, _polXYButton, _polYXButton, _polYYButton, _polIButton;
		
		Gtk::ToggleToolButton _amplitudeButton, _phaseButton, _realButton, _imaginaryButton;
		
		Gtk::ToggleToolButton _logarithmicButton, _zeroAxisButton;
		Gtk::ToolButton _plotPropertiesButton, _dataExportButton;
		
		const StatisticsCollection *_statCollection;
		Plot2D _plot;
		PlotWidget _plotWidget;
		
		class PlotPropertiesWindow *_plotPropertiesWindow;
		class DataWindow *_dataWindow;
		
		bool _customButtonsCreated;
		
		std::string getYDesc(const std::set<QualityTablesFormatter::StatisticKind>& kinds) const;
};

#endif
