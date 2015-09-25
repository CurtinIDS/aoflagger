#ifndef MULTIPLOT_H
#define MULTIPLOT_H

#include "plot.h"

#include "../msio/types.h"

#include <vector>

#include "../gui/plot/plotmanager.h"

class MultiPlot {
	public:
		MultiPlot(Plot2D &plot, size_t plotCount);
		~MultiPlot();

		void AddPoint(size_t plotIndex, num_t x, num_t y)
		{
			_points[plotIndex]->push_back(Point(x, y));
		}
		void SetLegend(int index, const std::string &title)
		{
			_legends[index] = title;
		}
		void Finish();
		Plot2D &Plot() { return _plot; }
		void SetXAxisText(const std::string text)
		{
			_xAxisText = text;
		}
		void SetYAxisText(const std::string text)
		{
			_yAxisText = text;
		}
	private:
		struct Point {
			Point(num_t _x, num_t _y) : x(_x), y(_y) { } 
			num_t x, y;
		};
		typedef std::vector<struct Point> PointList;
		std::string *_legends;
		PointList **_points;
		size_t _plotCount;
		Plot2D &_plot;
		std::string _xAxisText, _yAxisText;
};

#endif
