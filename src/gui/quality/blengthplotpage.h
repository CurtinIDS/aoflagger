#ifndef GUI_QUALITY__BLENGTHPLOTPAGE_H
#define GUI_QUALITY__BLENGTHPLOTPAGE_H

#include "twodimensionalplotpage.h"

#include "../../quality/statisticscollection.h"

#include "../../structures/measurementset.h"

#include "../../remote/clusteredobservation.h"

class BLengthPlotPage : public TwoDimensionalPlotPage {
	public:
    BLengthPlotPage() :
			_includeAutoCorrelationsButton("Auto-correlations")
		{
		}
	protected:
		virtual void processStatistics(const StatisticsCollection *statCollection, const std::vector<AntennaInfo> &antennas) override final
		{
			_statisticsWithAutocorrelations.clear();
			_statisticsWithoutAutocorrelations.clear();
			
			const BaselineStatisticsMap &map = statCollection->BaselineStatistics();
			
			vector<std::pair<unsigned, unsigned> > baselines = map.BaselineList();
			for(vector<std::pair<unsigned, unsigned> >::const_iterator i=baselines.begin();i!=baselines.end();++i)
			{
				Baseline bline(antennas[i->first], antennas[i->second]);
				const DefaultStatistics &statistics = map.GetStatistics(i->first, i->second);
				_statisticsWithAutocorrelations.insert(std::pair<double, DefaultStatistics>(bline.Distance(), statistics));
				if(i->first != i->second)
					_statisticsWithoutAutocorrelations.insert(std::pair<double, DefaultStatistics>(bline.Distance(), statistics));
			}
		}
		
		virtual const std::map<double, class DefaultStatistics> &getStatistics() const override final
		{
			return _includeAutoCorrelationsButton.get_active() ? _statisticsWithAutocorrelations : _statisticsWithoutAutocorrelations;
		}
		
		virtual void startLine(Plot2D &plot, const std::string &name, const std::string &yAxisDesc) override final
		{
			plot.StartLine(name, "Baseline length (m)", yAxisDesc, false, Plot2DPointSet::DrawPoints);
		}
		
		virtual void addCustomPlotButtons(Gtk::Toolbar &container) override final
		{
			_includeAutoCorrelationsButton.signal_clicked().connect(sigc::mem_fun(*this, &BLengthPlotPage::onAutoCorrelationsClicked));
			container.append(_includeAutoCorrelationsButton);
			_includeAutoCorrelationsButton.show();
		}
	private:
		void onAutoCorrelationsClicked()
		{
			updatePlot();
		}
		
		std::map<double, DefaultStatistics> _statisticsWithAutocorrelations;
		std::map<double, DefaultStatistics> _statisticsWithoutAutocorrelations;
		Gtk::ToggleToolButton _includeAutoCorrelationsButton;
};

#endif
