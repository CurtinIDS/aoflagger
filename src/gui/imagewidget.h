#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <gtkmm/drawingarea.h>

#include <cairomm/surface.h>

#include <vector>

#include "../structures/image2d.h"
#include "../structures/timefrequencydata.h"
#include "../structures/timefrequencymetadata.h"
#include "../structures/segmentedimage.h"

class ImageWidget : public Gtk::DrawingArea {
	public:
		enum TFMap { BWMap, InvertedMap, HotColdMap, RedBlueMap, RedYellowBlueMap, FireMap, BlackRedMap, ViridisMap };
		enum Range { MinMax, Winsorized, Specified };
		enum ScaleOption { NormalScale, LogScale, ZeroSymmetricScale };
		
		ImageWidget();
		~ImageWidget();

		bool ShowOriginalMask() const { return _showOriginalMask; }
		void SetShowOriginalMask(bool newValue) { _showOriginalMask = newValue; }

		bool ShowAlternativeMask() const { return _showAlternativeMask; }
		void SetShowAlternativeMask(bool newValue) { _showAlternativeMask = newValue; }

		TFMap GetColorMap() const { return _colorMap; }
		void SetColorMap(TFMap colorMap) { _colorMap = colorMap; }
		
		void SetRange(enum Range range)
		{
			_range = range;
		}
		enum Range Range() const
		{
			return _range;
		}
		void SetScaleOption(ScaleOption option)
		{
			_scaleOption = option;
		}
		enum ScaleOption ScaleOption() const { return _scaleOption; }
		
		void Update(); 

		Image2DCPtr Image() const { return _image; }
		void SetImage(Image2DCPtr image) { _image = image; }

		Mask2DCPtr OriginalMask() const { return _originalMask; }
		void SetOriginalMask(Mask2DCPtr mask) { _originalMask = mask; }

		Mask2DCPtr AlternativeMask() const { return _alternativeMask; }
		void SetAlternativeMask(Mask2DCPtr mask) { _alternativeMask = mask; }

		Mask2DCPtr GetActiveMask() const;

		void SetHighlighting(bool newValue) { _highlighting = newValue; }
		class ThresholdConfig &HighlightConfig() { return *_highlightConfig; }
		bool HasImage() const { return _image != 0; }
		void SetHorizontalDomain(double start, double end)
		{
			_startHorizontal = start;
			_endHorizontal = end;
			_onZoomChanged.emit();
		}
		void SetVerticalDomain(double start, double end)
		{
			_startVertical = start;
			_endVertical = end;
			_onZoomChanged.emit();
		}
		void ZoomFit();
		void ZoomIn();
		void ZoomInOn(size_t x, size_t y);
		void ZoomOut();
		double StartHorizontal() const { return _startHorizontal; }
		double EndHorizontal() const { return _endHorizontal; }
		double StartVertical() const { return _startVertical; }
		double EndVertical() const { return _endVertical; }
		void SetSegmentedImage(SegmentedImageCPtr segmentedImage) { _segmentedImage = segmentedImage; }
		TimeFrequencyMetaDataCPtr GetSelectedMetaData();
		const TimeFrequencyMetaDataCPtr& GetFullMetaData() const { return _metaData; }
		void SetMetaData(TimeFrequencyMetaDataCPtr metaData) { _metaData = metaData; }

		sigc::signal<void, size_t, size_t> &OnMouseMovedEvent() { return _onMouseMoved; }
		sigc::signal<void> &OnMouseLeaveEvent() { return _onMouseLeft; }
		sigc::signal<void, size_t, size_t> &OnButtonReleasedEvent() { return _onButtonReleased; }
		sigc::signal<void> &OnZoomChanged() { return _onZoomChanged; }
		
		num_t Max() const { return _max; }
		num_t Min() const { return _min; }
		
		void SetMax(num_t max) { _max = max; }
		void SetMin(num_t min) { _min = min; }
		
		void SaveByExtension(const std::string& filename, unsigned width=0, unsigned height=0);
		void SavePdf(const std::string &filename, unsigned width=0, unsigned height=0);
		void SaveSvg(const std::string &filename, unsigned width=0, unsigned height=0);
		void SavePng(const std::string &filename, unsigned width=0, unsigned height=0);
		void SaveText(const std::string &filename);
		
		bool ShowTitle() const { return _showTitle; }
		void SetShowTitle(bool showTitle) {
			_showTitle = showTitle;
		}
		
		bool ShowXYAxes() const { return _showXYAxes; }
		void SetShowXYAxes(bool showXYAxes)
		{
			_showXYAxes = showXYAxes;
		}
		
		bool ShowColorScale() const { return _showColorScale; }
		void SetShowColorScale(bool showColorScale)
		{
			_showColorScale = showColorScale;
		}
		
		bool ShowXAxisDescription() const { return _showXAxisDescription; }
		void SetShowXAxisDescription(bool showXAxisDescription)
		{
			_showXAxisDescription = showXAxisDescription;
		}
		
		bool ShowYAxisDescription() const { return _showYAxisDescription; }
		void SetShowYAxisDescription(bool showYAxisDescription)
		{
			_showYAxisDescription = showYAxisDescription;
		}
		
		bool ShowZAxisDescription() const { return _showZAxisDescription; }
		void SetShowZAxisDescription(bool showZAxisDescription)
		{
			_showZAxisDescription = showZAxisDescription;
		}
		
		void Clear();
		
		void SetCairoFilter(Cairo::Filter filter)
		{
			_cairoFilter = filter;
		}
		Cairo::Filter CairoFilter() const { return _cairoFilter; }
		void SetTitleText(const std::string &title)
		{
			_titleText = title;
		}
		
		const std::string& XAxisDescription() const { return _xAxisDescription; }
		void SetXAxisDescription(const std::string &description)
		{
			_xAxisDescription = description;
		}
		const std::string& YAxisDescription() const { return _yAxisDescription; }
		void SetYAxisDescription(const std::string &description)
		{
			_yAxisDescription = description;
		}
		const std::string& ZAxisDescription() const { return _zAxisDescription; }
		void SetZAxisDescription(const std::string &description)
		{
			_zAxisDescription = description;
		}
		
		bool ManualTitle() const { return _manualTitle; }
		void SetManualTitle(bool manualTitle) { _manualTitle = manualTitle; }
		
		const std::string &ManualTitleText() {
			return _manualTitleText;
		}
		void SetManualTitleText(const std::string &manualTitle) {
			_manualTitleText = manualTitle;
		}
		
		bool ManualXAxisDescription() const { return _manualXAxisDescription; }
		void SetManualXAxisDescription(bool manualDesc)
		{
			_manualXAxisDescription = manualDesc;
		}
		bool ManualYAxisDescription() const { return _manualYAxisDescription; }
		void SetManualYAxisDescription(bool manualDesc)
		{
			_manualYAxisDescription = manualDesc;
		}
		bool ManualZAxisDescription() const { return _manualZAxisDescription; }
		void SetManualZAxisDescription(bool manualDesc)
		{
			_manualZAxisDescription = manualDesc;
		}
		
		bool IsMouseInImage() const { return _mouseIsIn; }
		size_t MouseX() { return _mouseX; }
		size_t MouseY() { return _mouseY; }
		
		bool IsZoomedOut() const {
			return
				_startHorizontal == 0.0 &&
				_endHorizontal == 1.0 &&
				_startVertical == 0.0 &&
				_endVertical == 1.0;
		}

	private:
		void findMinMax(Image2DCPtr image, Mask2DCPtr mask, num_t &min, num_t &max);
		void update(Cairo::RefPtr<Cairo::Context> cairo, unsigned width, unsigned height);
		void redrawWithoutChanges(Cairo::RefPtr<Cairo::Context> cairo, unsigned width, unsigned height);
		void downsampleImageBuffer(unsigned newWidth, unsigned newHeight);
		bool toUnits(double mouseX, double mouseY, int &posX, int &posY);
		bool onDraw(const Cairo::RefPtr<Cairo::Context>& cr);
		bool onMotion(GdkEventMotion *event);
		bool onLeave(GdkEventCrossing *event);
		bool onButtonReleased(GdkEventButton *event);
		class ColorMap *createColorMap();
		std::string actualTitleText() const
		{
			if(_manualTitle)
				return _manualTitleText;
			else
				return _titleText;
		}

		bool _isInitialized;
		unsigned _initializedWidth, _initializedHeight;
		Cairo::RefPtr<Cairo::ImageSurface> _imageSurface;

		bool _showOriginalMask, _showAlternativeMask;
		enum TFMap _colorMap;
		TimeFrequencyMetaDataCPtr _metaData;
		Image2DCPtr _image;
		Mask2DCPtr _originalMask, _alternativeMask;
		bool _highlighting;
		class ThresholdConfig *_highlightConfig;
		double _leftBorderSize, _rightBorderSize, _topBorderSize, _bottomBorderSize;

		double _startHorizontal, _endHorizontal;
		double _startVertical, _endVertical;
		SegmentedImageCPtr _segmentedImage;
		class HorizontalPlotScale *_horiScale;
		class VerticalPlotScale *_vertScale;
		class ColorScale *_colorScale;
		class Title *_plotTitle;
		enum ScaleOption _scaleOption;
		bool _showXYAxes;
		bool _showColorScale;
		bool _showXAxisDescription;
		bool _showYAxisDescription;
		bool _showZAxisDescription;
		bool _showTitle;
		num_t _max, _min;
		std::string _titleText, _manualTitleText;
		enum Range _range;
		Cairo::Filter _cairoFilter;
		std::string _xAxisDescription, _yAxisDescription, _zAxisDescription;
		bool _manualTitle;
		bool _manualXAxisDescription;
		bool _manualYAxisDescription;
		bool _manualZAxisDescription;
		bool _mouseIsIn;
		size_t _mouseX, _mouseY;

		sigc::signal<void, size_t, size_t> _onMouseMoved;
		sigc::signal<void> _onMouseLeft;
		sigc::signal<void, size_t, size_t> _onButtonReleased;
		sigc::signal<void> _onZoomChanged;
};

#endif
