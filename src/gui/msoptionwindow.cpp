#include <iostream>

#include "gtkmm-compat.h"

#include "msoptionwindow.h"
#include "controllers/rfiguicontroller.h"

#include "../strategy/actions/strategy.h"

#include "../strategy/imagesets/msimageset.h"

#include "../strategy/control/defaultstrategy.h"

MSOptionWindow::MSOptionWindow(class RFIGuiController &controller, const std::string &filename) :
	Gtk::Window(),
	_controller(controller),
	_filename(filename),
	_openButton("_Open", true),
	_dataKindFrame("Columns to read"),
	_polarisationFrame("Polarisation to read"),
	_observedDataButton("Observed"), _correctedDataButton("Corrected"), _modelDataButton("Model"), _residualDataButton("Residual"),
	_otherColumnButton("Other:"),
	_allDipolePolarisationButton("Dipole (xx,xy,yx,yy separately)"),
	_autoDipolePolarisationButton("Dipole auto-correlations (xx and yy)"),
	_stokesIPolarisationButton("Stokes I"),
	_directReadButton("Direct IO"),
	_indirectReadButton("Indirect IO"),
	_memoryReadButton("Memory-mode IO"),
	_readUVWButton("Read UVW"),
	_loadOptimizedStrategy("Load optimized strategy")
{
	set_title("Options for opening a measurement set");

	initDataTypeButtons();
	initPolarisationButtons();

	gtkmm_set_image_from_icon_name(_openButton, "document-open");
	_openButton.signal_clicked().connect(sigc::mem_fun(*this, &MSOptionWindow::onOpen));
	_bottomButtonBox.pack_start(_openButton);

	_rightVBox.pack_start(_directReadButton);
	_rightVBox.pack_start(_indirectReadButton);
	_rightVBox.pack_start(_memoryReadButton);
	Gtk::RadioButton::Group group;
	_directReadButton.set_group(group);
	_indirectReadButton.set_group(group);
	_memoryReadButton.set_group(group);
	_directReadButton.set_active(true);

	_rightVBox.pack_start(_readUVWButton);
	_readUVWButton.set_active(true);

	_rightVBox.pack_start(_loadOptimizedStrategy);
	_loadOptimizedStrategy.set_active(true);

	_rightVBox.pack_start(_bottomButtonBox);

	_topHBox.pack_start(_leftVBox);
	_topHBox.pack_start(_rightVBox);

	add(_topHBox);
	show_all();
}

MSOptionWindow::~MSOptionWindow()
{
}

void MSOptionWindow::initDataTypeButtons()
{
	Gtk::RadioButton::Group group = _observedDataButton.get_group();
	_correctedDataButton.set_group(group);
	_modelDataButton.set_group(group);
	_residualDataButton.set_group(group);
	_otherColumnButton.set_group(group);

	_dataKindBox.pack_start(_observedDataButton);
	_dataKindBox.pack_start(_correctedDataButton);
	_dataKindBox.pack_start(_modelDataButton);
	_dataKindBox.pack_start(_residualDataButton);
	
	_otherColumnBox.pack_start(_otherColumnButton);
	_otherColumnBox.pack_start(_otherColumnEntry);
	_dataKindBox.pack_start(_otherColumnBox);

	_dataKindFrame.add(_dataKindBox);

	_leftVBox.pack_start(_dataKindFrame);
}

void MSOptionWindow::initPolarisationButtons()
{
	Gtk::RadioButton::Group group = _allDipolePolarisationButton.get_group();
	_autoDipolePolarisationButton.set_group(group);
	_stokesIPolarisationButton.set_group(group);

	_polarisationBox.pack_start(_allDipolePolarisationButton);
	_polarisationBox.pack_start(_autoDipolePolarisationButton);
	_polarisationBox.pack_start(_stokesIPolarisationButton);

	_polarisationFrame.add(_polarisationBox);
	_leftVBox.pack_start(_polarisationFrame);
}

void MSOptionWindow::onOpen()
{
	BaselineIOMode ioMode = DirectReadMode;
	if(_indirectReadButton.get_active()) ioMode = IndirectReadMode;
	else if(_memoryReadButton.get_active()) ioMode = MemoryReadMode;
	bool readUVW = _readUVWButton.get_active();
	
	std::string dataColumnName;
	bool subtractModel = false;
	if(_observedDataButton.get_active())
		dataColumnName = "DATA";
	else if(_correctedDataButton.get_active())
		dataColumnName = "CORRECTED_DATA";
	else if(_modelDataButton.get_active())
		dataColumnName = "MODEL_DATA";
	else if(_residualDataButton.get_active())
	{
		dataColumnName = "DATA";
		subtractModel = true;
	}
	else if(_otherColumnButton.get_active())
		dataColumnName = _otherColumnEntry.get_text();

	size_t polCount;
	if(_allDipolePolarisationButton.get_active())
		polCount = 4;
	else if(_autoDipolePolarisationButton.get_active())
		polCount = 2;
	else
		polCount = 1;
	
	bool loadStrategy = _loadOptimizedStrategy.get_active();
	
	_controller.Open(_filename, ioMode, readUVW, dataColumnName, subtractModel, polCount, true, loadStrategy);
	
	hide();
}
