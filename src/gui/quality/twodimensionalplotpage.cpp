#include <limits>

#include <boost/bind.hpp>

#include "datawindow.h"
#include "twodimensionalplotpage.h"

#include "../../quality/statisticscollection.h"
#include "../../quality/statisticsderivator.h"

#include "../plot/plotpropertieswindow.h"

TwoDimensionalPlotPage::TwoDimensionalPlotPage() :
	_expander("Side bar"),
	_statisticFrame("Statistics"),
	_countButton("Count"),
	_meanButton("Mean"),
	_stdDevButton("StdDev"),
	_varianceButton("Variance"),
	_dCountButton("DCount"),
	_dMeanButton("DMean"),
	_dStdDevButton("DStdDev"),
	_rfiPercentageButton("RFI"),
	_snrButton("SNR"),
	_polarizationFrame("Polarization"),
	_polXXButton("XX"),
	_polXYButton("XY"),
	_polYXButton("YX"),
	_polYYButton("YY"),
	_polXXandYYButton("XX/2+YY/2"),
	_polXYandYXButton("XY/2+YX/2"),
	_phaseFrame("Phase"),
	_amplitudeButton("Amplitude"),
	_phaseButton("Phase"),
	_realButton("Real"),
	_imaginaryButton("Imaginary"),
	_plotFrame("Plot"),
	_logarithmicButton("Logarithmic"),
	_zeroAxisButton("Zero axis"),
	_plotPropertiesButton("Properties..."),
	_dataExportButton("Data..."),
	_statCollection(0),
	_plotPropertiesWindow(0),
	_customButtonsCreated(false)
{
	initStatisticKindButtons();
	initPolarizationButtons();
	initPhaseButtons();
	initPlotButtons();
	
	_expander.add(_sideBox);
	pack_start(_expander, Gtk::PACK_SHRINK);
	
	_plotWidget.SetPlot(_plot);
	pack_start(_plotWidget, Gtk::PACK_EXPAND_WIDGET);
	
	show_all_children();
	
	_dataWindow = new DataWindow();
}

TwoDimensionalPlotPage::~TwoDimensionalPlotPage()
{
	delete _dataWindow;
	if(_plotPropertiesWindow != 0)
		delete _plotPropertiesWindow;
}

/*
unsigned TwoDimensionalPlotPage::selectedKindCount() const
{
	unsigned count = 0;
	if(_countButton.get_active()) ++count;
	if(_meanButton.get_active()) ++count;
	if(_stdDevButton.get_active()) ++count;
	if(_varianceButton.get_active()) ++count;
	if(_dCountButton.get_active()) ++count;
	if(_dMeanButton.get_active()) ++count;
	if(_dStdDevButton.get_active()) ++count;
	if(_rfiPercentageButton.get_active()) ++count;
	if(_snrButton.get_active()) ++count;
	return count;
}*/

void TwoDimensionalPlotPage::updatePlot()
{
	updatePlotForSettings(getSelectedKinds(), getSelectedPolarizations(), getSelectedPhases());
}

void TwoDimensionalPlotPage::updatePlotForSettings(
	const std::set<QualityTablesFormatter::StatisticKind>& kinds,
	const std::set<std::pair<unsigned int, unsigned int> >& pols,
	const std::set<PhaseType>& phases)
{
	if(HasStatistics())
	{
		_plot.Clear();
		
		for(std::set<QualityTablesFormatter::StatisticKind>::const_iterator k=kinds.begin();
				k!=kinds.end(); ++k)
		{
			for(std::set<std::pair<unsigned,unsigned> >::const_iterator p=pols.begin();
					p!=pols.end(); ++p)
			{
				for(std::set<PhaseType>::const_iterator ph=phases.begin();
						ph!=phases.end(); ++ph)
				{
					plotStatistic(*k, p->first, p->second, *ph, getYDesc(kinds));
				}
			}
		}
		
		processPlot(_plot);
		
		_plotWidget.Update();
		
		if(_dataWindow->get_visible())
		{
			updateDataWindow();
		}
	}
}

void TwoDimensionalPlotPage::updatePlotConfig()
{
	_plot.SetIncludeZeroYAxis(_zeroAxisButton.get_active());
	_plot.SetLogarithmicYAxis(_logarithmicButton.get_active());
	_plotWidget.Update();
}

double TwoDimensionalPlotPage::getValue(enum PhaseType phase, const std::complex<long double> val)
{
	switch(phase)
	{
		default:
		case AmplitudePhaseType: return sqrt(val.real()*val.real() + val.imag()*val.imag());
		case PhasePhaseType: return atan2(val.imag(), val.real());
		case RealPhaseType: return val.real();
		case ImaginaryPhaseType: return val.imag();
	}
}

std::set<QualityTablesFormatter::StatisticKind> TwoDimensionalPlotPage::getSelectedKinds() const
{
	std::set<QualityTablesFormatter::StatisticKind> kinds;
	if(_countButton.get_active())
		kinds.insert(QualityTablesFormatter::CountStatistic);
	if(_meanButton.get_active())
		kinds.insert(QualityTablesFormatter::MeanStatistic);
	if(_stdDevButton.get_active())
		kinds.insert(QualityTablesFormatter::StandardDeviationStatistic);
	if(_varianceButton.get_active())
		kinds.insert(QualityTablesFormatter::VarianceStatistic);
	if(_dCountButton.get_active())
		kinds.insert(QualityTablesFormatter::DCountStatistic);
	if(_dMeanButton.get_active())
		kinds.insert(QualityTablesFormatter::DMeanStatistic);
	if(_dStdDevButton.get_active())
		kinds.insert(QualityTablesFormatter::DStandardDeviationStatistic);
	if(_rfiPercentageButton.get_active())
		kinds.insert(QualityTablesFormatter::RFIPercentageStatistic);
	if(_snrButton.get_active())
		kinds.insert(QualityTablesFormatter::SignalToNoiseStatistic);
	return kinds;
}

std::set<std::pair<unsigned int, unsigned int> > TwoDimensionalPlotPage::getSelectedPolarizations() const
{
	std::set<std::pair<unsigned, unsigned> > pols;
	if(_polXXButton.get_active())
		pols.insert(std::make_pair(0, 0));
	if(_polXYButton.get_active())
		pols.insert(std::make_pair(1, 1));
	if(_polYXButton.get_active())
		pols.insert(std::make_pair(2, 2));
	if(_polYYButton.get_active())
		pols.insert(std::make_pair(3, 3));
	if(_polXXandYYButton.get_active())
		pols.insert(std::make_pair(0, 3));
	if(_polXYandYXButton.get_active())
		pols.insert(std::make_pair(1, 2));
	return pols;
}

std::set<TwoDimensionalPlotPage::PhaseType> TwoDimensionalPlotPage::getSelectedPhases() const
{
	std::set<TwoDimensionalPlotPage::PhaseType> phases;
	if(_amplitudeButton.get_active())
		phases.insert(AmplitudePhaseType);
	if(_phaseButton.get_active())
		phases.insert(PhasePhaseType);
	if(_realButton.get_active())
		phases.insert(RealPhaseType);
	if(_imaginaryButton.get_active())
		phases.insert(ImaginaryPhaseType);
	return phases;
}

void TwoDimensionalPlotPage::plotStatistic(QualityTablesFormatter::StatisticKind kind, unsigned polA, unsigned polB, PhaseType phase, const std::string& yDesc)
{
	std::ostringstream s;
	StatisticsDerivator derivator(*_statCollection);
	const std::map<double, DefaultStatistics> &statistics = getStatistics();
	if(polA == polB)
	{
		s << "Polarization " << polA;
		startLine(_plot, s.str(), yDesc);
		for(std::map<double, DefaultStatistics>::const_iterator i=statistics.begin();i!=statistics.end();++i)
		{
			const double x = i->first;
			const std::complex<long double> val = derivator.GetComplexStatistic(kind, i->second, polA);
			_plot.PushDataPoint(x, getValue(phase, val));
		}
	}
	else {
		s << "Polarization " << polA << " and " << polB;
		startLine(_plot, s.str(), yDesc);
		for(std::map<double, DefaultStatistics>::const_iterator i=statistics.begin();i!=statistics.end();++i)
		{
			const double x = i->first;
			const std::complex<long double>
				valA = derivator.GetComplexStatistic(kind, i->second, polA),
				valB = derivator.GetComplexStatistic(kind, i->second, polB),
				val = valA*0.5l + valB*0.5l;
			_plot.PushDataPoint(x, getValue(phase, val));
		}
	}
}

void TwoDimensionalPlotPage::initStatisticKindButtons()
{
	_countButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_countButton, Gtk::PACK_SHRINK);
	
	_meanButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_meanButton, Gtk::PACK_SHRINK);
	
	_stdDevButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_stdDevButton.set_active(true);
	_statisticBox.pack_start(_stdDevButton, Gtk::PACK_SHRINK);
	
	_varianceButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_varianceButton, Gtk::PACK_SHRINK);
	
	_dCountButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_dCountButton, Gtk::PACK_SHRINK);
	
	_dMeanButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_dMeanButton, Gtk::PACK_SHRINK);
	
	_dStdDevButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_dStdDevButton, Gtk::PACK_SHRINK);
	
	_rfiPercentageButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_rfiPercentageButton, Gtk::PACK_SHRINK);
	
	_snrButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_statisticBox.pack_start(_snrButton, Gtk::PACK_SHRINK);
	
	_statisticFrame.add(_statisticBox);
	
	_sideBox.pack_start(_statisticFrame, Gtk::PACK_SHRINK);
}

void TwoDimensionalPlotPage::initPolarizationButtons()
{
	_polXXButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_polarizationBox.pack_start(_polXXButton, Gtk::PACK_SHRINK);
	
	_polXYButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_polarizationBox.pack_start(_polXYButton, Gtk::PACK_SHRINK);
	
	_polYXButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_polarizationBox.pack_start(_polYXButton, Gtk::PACK_SHRINK);
	
	_polYYButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_polarizationBox.pack_start(_polYYButton, Gtk::PACK_SHRINK);
	
	_polXXandYYButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_polXXandYYButton.set_active(true);
	_polarizationBox.pack_start(_polXXandYYButton, Gtk::PACK_SHRINK);
	
	_polXYandYXButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_polarizationBox.pack_start(_polXYandYXButton, Gtk::PACK_SHRINK);
	
	_polarizationFrame.add(_polarizationBox);
	
	_sideBox.pack_start(_polarizationFrame, Gtk::PACK_SHRINK);
}

void TwoDimensionalPlotPage::initPhaseButtons()
{
	_amplitudeButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_amplitudeButton.set_active(true);
	_phaseBox.pack_start(_amplitudeButton, Gtk::PACK_SHRINK);
	
	_phaseButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_phaseBox.pack_start(_phaseButton, Gtk::PACK_SHRINK);
	
	_realButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_phaseBox.pack_start(_realButton, Gtk::PACK_SHRINK);
	
	_imaginaryButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlot));
	_phaseBox.pack_start(_imaginaryButton, Gtk::PACK_SHRINK);
	
	_phaseFrame.add(_phaseBox);
	
	_sideBox.pack_start(_phaseFrame, Gtk::PACK_SHRINK);
}

void TwoDimensionalPlotPage::initPlotButtons()
{
	_logarithmicButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::onLogarithmicClicked));
	_plotBox.pack_start(_logarithmicButton, Gtk::PACK_SHRINK);
	
	_zeroAxisButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlotConfig));
	_zeroAxisButton.set_active(true);
	_plotBox.pack_start(_zeroAxisButton, Gtk::PACK_SHRINK);
	_plot.SetIncludeZeroYAxis(true);
	
	_plotPropertiesButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::onPlotPropertiesClicked));
	_plotBox.pack_start(_plotPropertiesButton, Gtk::PACK_SHRINK);

	_dataExportButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::onDataExportClicked));
	_plotBox.pack_start(_dataExportButton, Gtk::PACK_SHRINK);
	
	_plotFrame.add(_plotBox);
	
	_sideBox.pack_start(_plotFrame, Gtk::PACK_SHRINK);
}

void TwoDimensionalPlotPage::onPlotPropertiesClicked()
{
	if(_plotPropertiesWindow == 0)
	{
		_plotPropertiesWindow = new PlotPropertiesWindow(_plot, "Plot properties");
		_plotPropertiesWindow->OnChangesApplied = boost::bind(&TwoDimensionalPlotPage::updatePlot, this);
	}
	
	_plotPropertiesWindow->show();
	_plotPropertiesWindow->raise();
}

void TwoDimensionalPlotPage::updateDataWindow()
{
	if(_dataWindow->get_visible())
		_dataWindow->SetData(_plot);
}

void TwoDimensionalPlotPage::onDataExportClicked()
{
	_dataWindow->show();
	_dataWindow->raise();
	updateDataWindow();
}

std::string TwoDimensionalPlotPage::getYDesc(const std::set<QualityTablesFormatter::StatisticKind>& kinds) const
{
	if(kinds.size() != 1)
		return "Value";
	else
	{
		QualityTablesFormatter::StatisticKind kind = *kinds.begin();
		return StatisticsDerivator::GetDescWithUnits(kind);
	}
}

void TwoDimensionalPlotPage::SavePdf(const string& filename, QualityTablesFormatter::StatisticKind kind)
{
	std::set<QualityTablesFormatter::StatisticKind> kinds;
	kinds.insert(kind);
	
	std::set<std::pair<unsigned int, unsigned int> > pols;
	pols.insert(std::make_pair(0, 3));
	
	std::set<PhaseType> phases;
	phases.insert(AmplitudePhaseType);

	updatePlotForSettings(kinds, pols, phases);
	
	_plot.SavePdf(filename);
}
