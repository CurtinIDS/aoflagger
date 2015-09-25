#include "../util/multiplot.h"

MultiPlot::MultiPlot(Plot2D &plot, size_t plotCount)
	: _plotCount(plotCount), _plot(plot)
{
	_points = new PointList*[plotCount];
	for(size_t i=0;i<plotCount;++i)
		_points[i] = new PointList();
	_legends = new std::string[plotCount];
}

MultiPlot::~MultiPlot()
{
	for(size_t i=0;i<_plotCount;++i)
		delete _points[i];
	delete[] _points;
	delete[] _legends;
}

void MultiPlot::Finish()
{
	for(size_t i=0;i<_plotCount;++i)
	{
		_plot.StartLine(_legends[i], _xAxisText, _yAxisText, false, Plot2DPointSet::DrawPoints);
		PointList &list = *_points[i];
		for(PointList::const_iterator p=list.begin();p!=list.end();++p)
		{
			_plot.PushDataPoint(p->x, p->y);
		}
		list.clear();
	}
}

