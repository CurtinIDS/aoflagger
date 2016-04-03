#ifndef GUI_QUALITY__TIMEPLOTPAGE_H
#define GUI_QUALITY__TIMEPLOTPAGE_H

#include "twodimensionalplotpage.h"

#include "../../quality/statisticscollection.h"

class TimePlotPage : public TwoDimensionalPlotPage {
	protected:
		virtual const std::map<double, class DefaultStatistics> &getStatistics() const override final
		{
			return getStatCollection()->TimeStatistics();
		}
		
		virtual void startLine(Plot2D &plot, const std::string &name, const std::string &yAxisDesc) override final
		{
			plot.StartLine(name, "Time", yAxisDesc, true);
		}
};

#endif
