#ifndef IMAGEPLANEWINDOW_H
#define IMAGEPLANEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/menu.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/radiomenuitem.h>

#include "../structures/timefrequencydata.h"
#include "../structures/timefrequencymetadata.h"

#include "../imaging/uvimager.h"

#include "imagewidget.h"

class ImagePlaneWindow : public Gtk::Window {
	public:
		ImagePlaneWindow();
		~ImagePlaneWindow();

		void AddData(const TimeFrequencyData &data, TimeFrequencyMetaDataCPtr metaData);
		void AddData(const TimeFrequencyData &data, class SpatialMatrixMetaData *spatialMetaData);
		UVImager *GetImager() throw() { return &_imager; }
		void Update();
	private:
		UVImager _imager;
		ImageWidget _imageWidget;
		Gtk::VBox _box;
		Gtk::HBox _topBox;
		Gtk::Button _clearButton;
		Gtk::Button _applyWeightsButton;
		Gtk::Button _refreshCurrentButton, _memoryStoreButton, _memoryRecallButton, _memoryMultiplyButton, _memorySubtractButton, _sqrtButton;
		Gtk::Button _plotHorizontalButton, _plotVerticalButton, _angularTransformButton, _saveFitsButton, _propertiesButton;
		Gtk::RadioButton _uvPlaneButton, _imagePlaneButton;
		Gtk::Menu _zoomMenu;
		Gtk::Button _zoomMenuButton;
		Gtk::RadioMenuItem::Group _zoomGroup;
		Gtk::RadioMenuItem
			_zoomXd4Button, _zoomXd2Button, _zoomX1Button, _zoomX2Button, _zoomX4Button, _zoomX8Button,
			_zoomX16Button, _zoomX32Button, _zoomX64Button, _zoomX128Button;
		long double _zoom;
		TimeFrequencyMetaDataCPtr _lastMetaData;	

		bool _displayingUV;
		Image2DCPtr _memory;
		class ImagePropertiesWindow *_propertiesWindow;
		
		void onClearClicked();
		void onApplyWeightsClicked();
		void onUVPlaneButtonClicked();
		void onImagePlaneButtonClicked();
		void onZoomMenuButtonClicked();
		void onZoomButtonClicked();
		void onRefreshCurrentClicked();
		void onMemoryStoreClicked();
		void onMemoryRecallClicked();
		void onMemoryMultiplyClicked();
		void onMemorySubtractClicked();
		void onSqrtClicked();
		void onPlotHorizontally();
		void onPlotVertically();
		void onAngularTransformButton();
		void onSaveFitsButton();
		void onPropertiesButton();
		void onButtonReleased(size_t x, size_t y);

		void printStats();
};

#endif
