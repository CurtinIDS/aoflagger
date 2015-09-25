#ifndef SYSTEM_H
#define SYSTEM_H

#include <map>
#include <string>

#include "dimension.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class System {
	public:
		System() : _includeZeroYAxis(false)
		{
		}

		~System()
		{
			Clear();
		}

		void AddToSystem(class Plot2DPointSet &pointSet)
		{
			Dimension *dimension;
			if(_dimensions.count(pointSet.YUnits()) == 0)
			{
				dimension = new Dimension();
				_dimensions.insert(std::pair<std::string, Dimension*>(pointSet.YUnits(), dimension));
			} else {
				dimension = _dimensions.find(pointSet.YUnits())->second;
			}
			dimension->AdjustRanges(pointSet);
		}

		double XRangeMin(class Plot2DPointSet &pointSet) const
		{
			return _dimensions.find(pointSet.YUnits())->second->XRangeMin();
		}
		double XRangeMax(class Plot2DPointSet &pointSet) const
		{
			return _dimensions.find(pointSet.YUnits())->second->XRangeMax();
		}
		double YRangeMin(class Plot2DPointSet &pointSet) const
		{
			const double yMin = _dimensions.find(pointSet.YUnits())->second->YRangeMin();
			if(yMin > 0.0 && _includeZeroYAxis)
				return 0.0;
			else
				return yMin;
		}
		double YRangePositiveMin(class Plot2DPointSet &pointSet) const
		{
			return _dimensions.find(pointSet.YUnits())->second->YRangePositiveMin();
		}
		double YRangeMax(class Plot2DPointSet &pointSet) const
		{
			const double yMax = _dimensions.find(pointSet.YUnits())->second->YRangeMax();
			if(yMax < 0.0 && _includeZeroYAxis)
				return 0.0;
			else
				return yMax;
		}
		double YRangePositiveMax(class Plot2DPointSet &pointSet) const
		{
			return _dimensions.find(pointSet.YUnits())->second->YRangePositiveMax();
		}
		void Clear()
		{
			for(std::map<std::string, Dimension*>::iterator i=_dimensions.begin();i!=_dimensions.end();++i)
				delete i->second;
			_dimensions.clear();
		}
		void SetIncludeZeroYAxis(bool includeZeroYAxis) { _includeZeroYAxis = includeZeroYAxis; }
	private:
		std::map<std::string, Dimension*> _dimensions;
		bool _includeZeroYAxis;
};

#endif
