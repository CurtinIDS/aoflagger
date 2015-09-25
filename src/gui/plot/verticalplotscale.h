#ifndef VERTICALPLOTSCALE_H
#define VERTICALPLOTSCALE_H

#include <string>
#include <vector>

#include <gtkmm/drawingarea.h>

#include "tickset.h"

class VerticalPlotScale {
	public:
		VerticalPlotScale();
		virtual ~VerticalPlotScale();
		void SetPlotDimensions(double plotWidth, double plotHeight, double topMargin)
		{
			_plotWidth = plotWidth;
			_plotHeight = plotHeight;
			_topMargin = topMargin;
			_metricsAreInitialized = false;
		}
		
		double GetWidth(Cairo::RefPtr<Cairo::Context> cairo)
		{
			initializeMetrics(cairo);
			return _width;
		}
		
		double GetTextHeight(Cairo::RefPtr<Cairo::Context> cairo);
		
		void SetDrawWithDescription(bool drawWithDescription)
		{
			_drawWithDescription = drawWithDescription;
			_metricsAreInitialized = false;
		}
		void SetUnitsCaption(const std::string &caption)
		{
			_unitsCaption = caption;
			_metricsAreInitialized = false;
		}
		void SetDescriptionFontSize(double fontSize)
		{
			_tickValuesFontSize = fontSize;
			_metricsAreInitialized = false;
		}
		void SetTickValuesFontSize(double fontSize)
		{
			_tickValuesFontSize = fontSize;
			_metricsAreInitialized = false;
		}
		
		void Draw(Cairo::RefPtr<Cairo::Context> cairo, double offsetX=0.0, double offsetY=0.0);
		void InitializeNumericTicks(double min, double max);
		void InitializeLogarithmicTicks(double min, double max);
	private:
		void drawUnits(Cairo::RefPtr<Cairo::Context> cairo, double offsetX, double offsetY);
		bool ticksFit(Cairo::RefPtr<Cairo::Context> cairo);
		void initializeMetrics(Cairo::RefPtr<Cairo::Context> cairo); 
		double getTickYPosition(const Tick &tick);

		double _plotWidth, _plotHeight, _topMargin;
		bool _metricsAreInitialized;
		double _width;
		class TickSet *_tickSet;
		bool _isLogarithmic;
		bool _drawWithDescription;
		std::string _unitsCaption;
		double _descriptionFontSize;
		double _tickValuesFontSize;
};

#endif
