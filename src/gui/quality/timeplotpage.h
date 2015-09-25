#ifndef GUI_QUALITY__TIMEPLOTPAGE_H
#define GUI_QUALITY__TIMEPLOTPAGE_H

#include "twodimensionalplotpage.h"

#include "../../quality/statisticscollection.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class TimePlotPage : public TwoDimensionalPlotPage {
	protected:
		virtual const std::map<double, class DefaultStatistics> &GetStatistics() const
		{
			return GetStatCollection()->TimeStatistics();
		}
		
		virtual void StartLine(Plot2D &plot, const std::string &name, const std::string &yAxisDesc)
		{
			plot.StartLine(name, "Time", yAxisDesc, true);
		}
};

#endif
