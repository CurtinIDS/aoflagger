#ifndef FOREACHPOLARISATIONFRAME_H
#define FOREACHPOLARISATIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>

#include "../../strategy/actions/foreachpolarisationaction.h"

#include "../editstrategywindow.h"

class ForEachPolarisationFrame : public Gtk::Frame {
	public:
		ForEachPolarisationFrame(rfiStrategy::ForEachPolarisationBlock &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("For each polarisation"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_onXXButton("XX"),
		_onXYButton("XY"),
		_onYXButton("YX"),
		_onYYButton("YY"),
		_onStokesIButton("Stokes I"),
		_onStokesQButton("Stokes Q"),
		_onStokesUButton("Stokes U"),
		_onStokesVButton("Stokes V"),
		_applyButton("Apply")
		{
			_box.pack_start(_onXXButton);
			_onXXButton.set_active(_action.OnXX());
			_onXXButton.show();

			_box.pack_start(_onXYButton);
			_onXYButton.set_active(_action.OnXY());
			_onXYButton.show();

			_box.pack_start(_onYXButton);
			_onYXButton.set_active(_action.OnYX());
			_onYXButton.show();

			_box.pack_start(_onYYButton);
			_onYYButton.set_active(_action.OnYY());
			_onYYButton.show();

			_box.pack_start(_onStokesIButton);
			_onStokesIButton.set_active(_action.OnStokesI());
			_onStokesIButton.show();

			_box.pack_start(_onStokesQButton);
			_onStokesQButton.set_active(_action.OnStokesQ());
			_onStokesQButton.show();

			_box.pack_start(_onStokesUButton);
			_onStokesUButton.set_active(_action.OnStokesU());
			_onStokesUButton.show();

			_box.pack_start(_onStokesVButton);
			_onStokesVButton.set_active(_action.OnStokesV());
			_onStokesVButton.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &ForEachPolarisationFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::ForEachPolarisationBlock &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::CheckButton _onXXButton;
		Gtk::CheckButton _onXYButton;
		Gtk::CheckButton _onYXButton;
		Gtk::CheckButton _onYYButton;
		Gtk::CheckButton _onStokesIButton;
		Gtk::CheckButton _onStokesQButton;
		Gtk::CheckButton _onStokesUButton;
		Gtk::CheckButton _onStokesVButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetOnXX(_onXXButton.get_active());
			_action.SetOnXY(_onXYButton.get_active());
			_action.SetOnYX(_onYXButton.get_active());
			_action.SetOnYY(_onYYButton.get_active());
			_action.SetOnStokesI(_onStokesIButton.get_active());
			_action.SetOnStokesQ(_onStokesQButton.get_active());
			_action.SetOnStokesU(_onStokesUButton.get_active());
			_action.SetOnStokesV(_onStokesVButton.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // FOREACHPOLARISATIONFRAME_H
