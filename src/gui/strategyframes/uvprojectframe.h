#ifndef UV_PROJECT_FRAME_H
#define UV_PROJECT_FRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/uvprojectaction.h"

#include "../editstrategywindow.h"

class UVProjectFrame : public Gtk::Frame {
	public:
		UVProjectFrame(rfiStrategy::UVProjectAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("UV project"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_onRevisedButton("On revised"),
		_onContaminatedButton("On contaminated"),
		_angleLabel("Angle: (degrees)"),
		_angleScale(Gtk::ORIENTATION_HORIZONTAL),
		_etaLabel("Eta: (ratio)"),
		_etaScale(Gtk::ORIENTATION_HORIZONTAL),
		_reverseButton("Reverse"),
		_applyButton("Apply")
		{
			_box.pack_start(_onRevisedButton);
			_onRevisedButton.set_active(action.OnRevised());
			_onRevisedButton.show();
			
			_box.pack_start(_onContaminatedButton);
			_onContaminatedButton.set_active(action.OnContaminated());
			_onContaminatedButton.show();
			
			_box.pack_start(_angleLabel);
			_angleLabel.show();

			_box.pack_start(_angleScale);
			_angleScale.set_range(-180, 180);
			_angleScale.set_increments(0.1, 1.0);
			_angleScale.set_value(action.DirectionRad()*180.0/M_PI);
			_angleScale.show();
			
			_box.pack_start(_etaLabel);
			_etaLabel.show();

			_box.pack_start(_etaScale);
			_etaScale.set_range(0.0, 1.0);
			_etaScale.set_increments(0.01, 0.1);
			_etaScale.set_value(action.EtaParameter());
			_etaScale.show();
			
			_box.pack_start(_reverseButton);
			_reverseButton.set_active(action.Reverse());
			_reverseButton.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &UVProjectFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::UVProjectAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::CheckButton _onRevisedButton, _onContaminatedButton;
		Gtk::Label _angleLabel;
		Gtk::Scale _angleScale;
		Gtk::Label _etaLabel;
		Gtk::Scale _etaScale;
		Gtk::CheckButton _reverseButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetOnRevised(_onRevisedButton.get_active());
			_action.SetOnContaminated(_onContaminatedButton.get_active());
			_action.SetDirectionRad((num_t) _angleScale.get_value()/180.0*M_PI);
			_action.SetEtaParameter(_etaScale.get_value());
			_action.SetReverse(_reverseButton.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // UV_PROJECT_FRAME_H
