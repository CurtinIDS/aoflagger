#ifndef GUI_QUALITY__ANTENNAEPLOTPAGE_H
#define GUI_QUALITY__ANTENNAEPLOTPAGE_H

#include "twodimensionalplotpage.h"

#include "../../quality/statisticscollection.h"

#include "../../structures/measurementset.h"

class AntennaePlotPage : public TwoDimensionalPlotPage {
	protected:
		virtual void processStatistics(class StatisticsCollection *statCollection, const std::vector<AntennaInfo> &antennas)
		{
			_antennas = antennas;
			const BaselineStatisticsMap &map = statCollection->BaselineStatistics();
			
			vector<std::pair<unsigned, unsigned> > baselines = map.BaselineList();
			for(vector<std::pair<unsigned, unsigned> >::const_iterator i=baselines.begin();i!=baselines.end();++i)
			{
				if(i->first != i->second)
				{
					const DefaultStatistics& stats = map.GetStatistics(i->first, i->second);
					addStatistic(i->first, stats);
					addStatistic(i->second, stats);
				}
			}
		}
		
		void addStatistic(unsigned antIndex, const DefaultStatistics& stats)
		{
			std::map<double, DefaultStatistics>::iterator iter = _statistics.find(antIndex);
			if(iter == _statistics.end())
				_statistics.insert(std::pair<double, DefaultStatistics>(antIndex, stats));
			else
				iter->second += stats;
		}
		
		virtual const std::map<double, class DefaultStatistics> &GetStatistics() const
		{
			return _statistics;
		}
		
		virtual void StartLine(Plot2D &plot, const std::string &name, const std::string &yAxisDesc)
		{
			Plot2DPointSet &pointSet = plot.StartLine(name, "Antenna index", yAxisDesc, false, Plot2DPointSet::DrawColumns);
			
			std::vector<std::string> labels;
			for(std::vector<AntennaInfo>::const_iterator i=_antennas.begin();i!=_antennas.end();++i)
				labels.push_back(i->name);
			pointSet.SetTickLabels(labels);
			pointSet.SetRotateUnits(true);
		}
		
	private:
		std::map<double, DefaultStatistics> _statistics;
		std::vector<AntennaInfo> _antennas;
};

#endif
