#ifndef MSOPTIONWINDOW_H
#define MSOPTIONWINDOW_H

#include <string>

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class MSOptionWindow : public Gtk::Window {
	public:
		MSOptionWindow(class RFIGuiWindow &rfiGuiWindow, class StrategyController &strategyController, const std::string &filename);
		~MSOptionWindow();
		void onOpen();
	private:
		void initDataTypeButtons();
		void initPolarisationButtons();

		class RFIGuiWindow &_rfiGuiWindow;
		class StrategyController &_strategyController;
		const std::string _filename;

		Gtk::ButtonBox _bottomButtonBox;
		Gtk::VBox _leftVBox, _rightVBox;
		Gtk::HBox _topHBox;
		Gtk::Button _openButton;
		Gtk::Frame _dataKindFrame, _polarisationFrame;
		Gtk::VBox _dataKindBox, _polarisationBox;
		Gtk::HBox _otherColumnBox;
		Gtk::RadioButton _observedDataButton, _correctedDataButton, _modelDataButton, _residualDataButton, _otherColumnButton;
		Gtk::Entry _otherColumnEntry;
		Gtk::RadioButton _allDipolePolarisationButton, _autoDipolePolarisationButton, _stokesIPolarisationButton;
		Gtk::RadioButton _directReadButton, _indirectReadButton, _memoryReadButton;
		Gtk::CheckButton _readUVWButton, _loadOptimizedStrategy;
};

#endif
