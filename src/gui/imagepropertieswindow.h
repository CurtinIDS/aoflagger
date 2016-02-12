#ifndef IMAGEPROPERTIESWINDOW_H
#define IMAGEPROPERTIESWINDOW_H

#include <string>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/scale.h>
#include <gtkmm/window.h>

class ImagePropertiesWindow : public Gtk::Window {
	public:
		ImagePropertiesWindow(class ImageWidget &imageWidget, const std::string &title);
		~ImagePropertiesWindow() { }
	private:
		void onApplyClicked();
		void onCloseClicked();
		void onExportClicked();
		void onExportDataClicked();
		
		void onScaleChanged()
		{
			_scaleMinEntry.set_sensitive(_specifiedScaleButton.get_active());
			_scaleMaxEntry.set_sensitive(_specifiedScaleButton.get_active());
		}
		
		void initColorMapButtons();
		void initScaleWidgets();
		void initOptionsWidgets();
		void initFilterWidgets();
		void initZoomWidgets();
		void initAxisWidgets();
		
		void updateMinMaxEntries();

		class ImageWidget &_imageWidget;

		Gtk::ButtonBox _bottomButtonBox;
		Gtk::VBox _topVBox;
		Gtk::HBox _framesHBox;
		
		Gtk::Button _applyButton, _exportButton, _exportDataButton, _closeButton;
		Gtk::Frame _colorMapFrame;
		Gtk::VBox _colorMapBox;
		Gtk::RadioButton _grayScaleButton, _invGrayScaleButton, _hotColdScaleButton, _redBlueScaleButton, _blackRedScaleButton, _redBlueYellowScaleButton, _fireScaleButton, _viridisScaleButton;
		
		Gtk::Frame _scaleFrame;
		Gtk::VBox _scaleBox;
		Gtk::RadioButton _minMaxScaleButton, _winsorizedScaleButton, _specifiedScaleButton;
		Gtk::Label _scaleMinLabel, _scaleMaxLabel;
		Gtk::Entry _scaleMinEntry, _scaleMaxEntry;
		
		Gtk::VBox _filterAndOptionsBox;
		
		Gtk::Frame _optionsFrame;
		Gtk::VBox _optionsBox;
		Gtk::RadioButton _normalOptionsButton, _logScaleButton, _zeroSymmetricButton;
		
		Gtk::Frame _filterFrame;
		Gtk::VBox _filterBox;
		Gtk::RadioButton _bestFilterButton, _nearestFilterButton;
		
		Gtk::Frame _zoomFrame;
		Gtk::HBox _zoomHBox;
		Gtk::VBox _zoomVSubBox;
		Gtk::Scale _hStartScale, _hStopScale;
		Gtk::Scale _vStartScale, _vStopScale;
		
		Gtk::Frame _axesFrame;
		Gtk::HBox _axesHBox;
		Gtk::VBox _axesGeneralBox;
		Gtk::VBox _axesVisibilityBox;
		Gtk::CheckButton _showXYAxes, _showColorScale;
		Gtk::HBox _titleBox, _xAxisBox, _yAxisBox, _zAxisBox;
		Gtk::CheckButton _showTitleButton, _showXAxisDescriptionButton, _showYAxisDescriptionButton, _showZAxisDescriptionButton;
		Gtk::CheckButton _manualTitle, _manualXAxisDescription, _manualYAxisDescription, _manualZAxisDescription;
		Gtk::Entry _titleEntry, _xAxisDescriptionEntry, _yAxisDescriptionEntry, _zAxisDescriptionEntry;
};

#endif // IMAGEPROPERTIESWINDOW_H
