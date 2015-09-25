#ifndef DIMENSION_H
#define DIMENSION_H

#include "plot2dpointset.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class Dimension {
	public:
		Dimension() : _pointSets(0) { }
		~Dimension() { }

		void AdjustRanges(Plot2DPointSet &pointSet)
		{
			if(_pointSets == 0)
			{
				_xRangeMin = pointSet.XRangeMin();
				_xRangeMax = pointSet.XRangeMax();
				_yRangeMin = pointSet.YRangeMin();
				_yRangeMax = pointSet.YRangeMax();
				_yRangePositiveMin = pointSet.YRangePositiveMin();
				_yRangePositiveMax = pointSet.YRangePositiveMax();
			} else {
				if(_xRangeMin > pointSet.XRangeMin() && std::isfinite(pointSet.XRangeMin()))
					_xRangeMin = pointSet.XRangeMin();
				if(_xRangeMax < pointSet.XRangeMax() && std::isfinite(pointSet.XRangeMax()))
					_xRangeMax = pointSet.XRangeMax();
				
				if(_yRangeMin > pointSet.YRangeMin() && std::isfinite(pointSet.YRangeMin()))
					_yRangeMin = pointSet.YRangeMin();
				if(_yRangePositiveMin > pointSet.YRangePositiveMin() && std::isfinite(pointSet.YRangePositiveMin())) 
					_yRangePositiveMin = pointSet.YRangePositiveMin();
				
				if(_yRangeMax < pointSet.YRangeMax() && std::isfinite(pointSet.YRangeMax()))
					_yRangeMax = pointSet.YRangeMax();
				if(_yRangePositiveMax < pointSet.YRangePositiveMax() && std::isfinite(pointSet.YRangePositiveMax())) 
					_yRangePositiveMin = pointSet.YRangePositiveMax();
			}
			++_pointSets;
		}

		double XRangeMin() const { return _xRangeMin; }
		double XRangeMax() const { return _xRangeMax; }
		double YRangeMin() const { return _yRangeMin; }
		double YRangeMax() const { return _yRangeMax; }
		double YRangePositiveMin() const { return _yRangePositiveMin; }
		double YRangePositiveMax() const { return _yRangePositiveMax; }
	private:
		size_t _pointSets;
		double _xRangeMin, _xRangeMax;
		double _yRangeMin, _yRangeMax;
		double _yRangePositiveMin, _yRangePositiveMax;
};

#endif
