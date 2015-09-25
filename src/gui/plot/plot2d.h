#ifndef PLOT2D_H
#define PLOT2D_H

#include <gtkmm/drawingarea.h>

#include <stdexcept>
#include <string>

#include "plotable.h"
#include "plot2dpointset.h"
#include "system.h"
#include "horizontalplotscale.h"
#include "verticalplotscale.h"

class Plot2D : public Plotable {
	public:
		enum RangeDetermination { MinMaxRange, WinsorizedRange, SpecifiedRange };
		
		Plot2D();
		~Plot2D();

		void Clear();
		Plot2DPointSet &StartLine(const std::string &label, const std::string &xDesc = "x", const std::string &yDesc = "y", bool xIsTime = false, enum Plot2DPointSet::DrawingStyle drawingStyle = Plot2DPointSet::DrawLines)
		{
			Plot2DPointSet *newSet = new Plot2DPointSet();
			newSet->SetLabel(label);
			newSet->SetXIsTime(xIsTime);
			newSet->SetXDesc(xDesc);
			newSet->SetYDesc(yDesc);
			newSet->SetDrawingStyle(drawingStyle);
			_pointSets.push_back(newSet);
			return *newSet;
		}
		Plot2DPointSet &StartLine(const std::string &label, enum Plot2DPointSet::DrawingStyle drawingStyle)
		{
			return StartLine(label, "x", "y", false, drawingStyle);
		}
		Plot2DPointSet &StartLine()
		{
			return StartLine("", "x", "y", false, Plot2DPointSet::DrawLines);
		}
		void PushDataPoint(double x, double y)
		{
			if(_pointSets.size() > 0)
				(*_pointSets.rbegin())->PushDataPoint(x,y);
			else
				throw std::runtime_error("Trying to push a data point into a plot without point sets (call StartLine first).");
		}
		size_t PointSetCount() const { return _pointSets.size(); }
		Plot2DPointSet &GetPointSet(size_t index) { return *_pointSets[index]; }
		const Plot2DPointSet &GetPointSet(size_t index) const { return *_pointSets[index]; }
		virtual void Render(Gtk::DrawingArea &drawingArea);
		void SetIncludeZeroYAxis(bool includeZeroAxis)
		{
			_system.SetIncludeZeroYAxis(includeZeroAxis);
			if(includeZeroAxis)
				_logarithmicYAxis = false;
		}
		void SetLogarithmicYAxis(bool logarithmicYAxis)
		{
			_logarithmicYAxis = logarithmicYAxis;
			if(_logarithmicYAxis)
				_system.SetIncludeZeroYAxis(false);
		}
		bool LogarithmicYAxis() const
		{
			return _logarithmicYAxis;
		}
		void SetHRangeDetermination(enum RangeDetermination range) {
			_hRangeDetermination = range;
		}
		enum RangeDetermination HRangeDetermination() const
		{
			return _hRangeDetermination;
		}
		void SetVRangeDetermination(enum RangeDetermination range) {
			_vRangeDetermination = range;
		}
		enum RangeDetermination VRangeDetermination() const
		{
			return _vRangeDetermination;
		}
		void SetMaxX(double maxX)
		{
			_hRangeDetermination = SpecifiedRange;
			_specifiedMaxX = maxX;
		}
		double MaxX() const
		{
			if(_hRangeDetermination == SpecifiedRange)
				return _specifiedMaxX;
			else if(_pointSets.empty())
				return 1.0;
			else
				return _system.XRangeMax(**_pointSets.begin());
		}
		void SetMinX(double minX)
		{
			_hRangeDetermination = SpecifiedRange;
			_specifiedMinX = minX;
		}
		double MinX() const
		{
			if(_hRangeDetermination == SpecifiedRange)
				return _specifiedMinX;
			else if(_pointSets.empty())
				return 1.0;
			else
				return _system.XRangeMin(**_pointSets.begin());
		}
		void SetMaxY(double maxY)
		{
			_vRangeDetermination = SpecifiedRange;
			_specifiedMaxY = maxY;
		}
		double MaxY() const
		{
			if(_vRangeDetermination == SpecifiedRange)
				return _specifiedMaxY;
			else if(_pointSets.empty())
				return 1.0;
			else
				return _system.YRangeMax(**_pointSets.begin());
		}
		double MaxPositiveY() const
		{
			if(_vRangeDetermination == SpecifiedRange)
				return _specifiedMaxY;
			else if(_pointSets.empty())
				return 1.0;
			else
				return _system.YRangePositiveMax(**_pointSets.begin());
		}
		void SetMinY(double minY)
		{
			_vRangeDetermination = SpecifiedRange;
			_specifiedMinY = minY;
		}
		double MinY() const
		{
			if(_vRangeDetermination == SpecifiedRange)
				return _specifiedMinY;
			else if(_pointSets.empty())
				return -1.0;
			else
				return _system.YRangeMin(**_pointSets.begin());
		}
		double MinPositiveY() const
		{
			if(_vRangeDetermination == SpecifiedRange)
				return _specifiedMinY;
			else if(_pointSets.empty())
				return 0.1;
			else
				return _system.YRangePositiveMin(**_pointSets.begin());
		}
		void SetShowAxes(bool showAxes) {
			_showAxes = showAxes;
		}
		bool ShowAxes() const {
			return _showAxes;
		}
		void SetShowAxisDescriptions(bool showAxisDescriptions) {
			_showAxisDescriptions = showAxisDescriptions;
		}
		bool ShowAxisDescriptions() const {
			return _showAxisDescriptions;
		}
		void SetTitle(const std::string &title) { _title = title; }
		void SetCustomHorizontalAxisDescription(const std::string& description) { _customHAxisDescription = description; }
		void SetCustomVerticalAxisDescription(const std::string& description) { _customVAxisDescription = description; }
		void SetAutomaticHorizontalAxisDescription() { _customHAxisDescription = std::string(); }
		void SetAutomaticVerticalAxisDescription() { _customVAxisDescription = std::string(); }
		void SavePdf(const std::string &filename);
		void SaveSvg(const std::string &filename);
		void SavePng(const std::string &filename);
		
		const std::string &Title() const { return _title; }
	private:
		void render(Cairo::RefPtr<Cairo::Context> cr);
		void render(Cairo::RefPtr<Cairo::Context> cr, Plot2DPointSet &pointSet);

		HorizontalPlotScale _horizontalScale;
		VerticalPlotScale _verticalScale;
		std::vector<Plot2DPointSet*> _pointSets;
		int _width, _height;
		double _topMargin;
		System _system;
		bool _logarithmicYAxis, _showAxes, _showAxisDescriptions;
		double _specifiedMinX, _specifiedMaxX, _specifiedMinY, _specifiedMaxY;
		enum RangeDetermination _hRangeDetermination, _vRangeDetermination;
		std::string _title, _customHAxisDescription, _customVAxisDescription;
};

#endif
