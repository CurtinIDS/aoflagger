#ifndef FRINGESTOPPINGFRAME_H
#define FRINGESTOPPINGFRAME_H

#include <sstream>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/fringestopaction.h"

#include "../editstrategywindow.h"

class FringeStoppingFrame : public Gtk::Frame {
	public:
		FringeStoppingFrame(rfiStrategy::FringeStopAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Fringe stopping recovery"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_fringesToConsiderLabel("Considered fringes:"),
		_fringesToConsiderScale(Gtk::ORIENTATION_HORIZONTAL),
		_minWindowSizeLabel("Min window size:"),
		_minWindowSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_maxWindowSizeLabel("Max window size:"),
		_maxWindowSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_fitChannelsIndividuallyButton("Fit channels individually"),
		_onlyFringeStopButton("No fit, only fringe stop"),
		_raLabel("Right ascension:"),
		_decLabel("Declination:"),
		_applyButton("Apply")
		{
			_box.pack_start(_fringesToConsiderLabel);

			_box.pack_start(_fringesToConsiderScale);
			_fringesToConsiderScale.set_range(0, 25.0L);
			_fringesToConsiderScale.set_value(_action.FringesToConsider());

			_box.pack_start(_minWindowSizeLabel);

			_box.pack_start(_minWindowSizeScale);
			_minWindowSizeScale.set_range(0, 2048);
			_minWindowSizeScale.set_value(_action.MinWindowSize());

			_box.pack_start(_maxWindowSizeLabel);

			_box.pack_start(_maxWindowSizeScale);
			_maxWindowSizeScale.set_range(0, 2048);
			_maxWindowSizeScale.set_value(_action.MaxWindowSize());

			_box.pack_start(_fitChannelsIndividuallyButton);
			_fitChannelsIndividuallyButton.set_active(_action.FitChannelsIndividually());

			_box.pack_start(_onlyFringeStopButton);
			_onlyFringeStopButton.set_active(_action.OnlyFringeStop());
			
			_box.pack_start(_raLabel);
			
			std::ostringstream raStr;
			raStr << _action.NewPhaseCentreRA();
			_raEntry.set_text(raStr.str());
			_box.pack_start(_raEntry);

			_box.pack_start(_decLabel);

			std::ostringstream decStr;
			decStr << _action.NewPhaseCentreDec();
			_decEntry.set_text(decStr.str());
			_box.pack_start(_decEntry);

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &FringeStoppingFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::FringeStopAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _fringesToConsiderLabel;
		Gtk::Scale _fringesToConsiderScale;
		Gtk::Label _minWindowSizeLabel;
		Gtk::Scale _minWindowSizeScale;
		Gtk::Label _maxWindowSizeLabel;
		Gtk::Scale _maxWindowSizeScale;
		Gtk::CheckButton _fitChannelsIndividuallyButton;
		Gtk::CheckButton _onlyFringeStopButton;
		Gtk::Label _raLabel;
		Gtk::Label _decLabel;
		Gtk::Entry _raEntry, _decEntry;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetFringesToConsider(_fringesToConsiderScale.get_value());
			_action.SetMinWindowSize((size_t) _minWindowSizeScale.get_value());
			_action.SetMaxWindowSize((size_t) _maxWindowSizeScale.get_value());
			_action.SetFitChannelsIndividually(_fitChannelsIndividuallyButton.get_active());
			_action.SetOnlyFringeStop(_onlyFringeStopButton.get_active());
			std::string
				raStr = _raEntry.get_text(),
				decStr = _decEntry.get_text();
			_action.SetNewPhaseCentreRA(atof(raStr.c_str()));
			_action.SetNewPhaseCentreDec(atof(decStr.c_str()));
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // FRINGESTOPPINGFRAME_H
