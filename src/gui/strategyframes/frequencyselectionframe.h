#ifndef FREQUENCYSELECTIONFRAME_H
#define FREQUENCYSELECTIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/frequencyselectionaction.h"

#include "../editstrategywindow.h"

class FrequencySelectionFrame : public Gtk::Frame {
	public:
		FrequencySelectionFrame(rfiStrategy::FrequencySelectionAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Frequency selection"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_thresholdLabel("Threshold:"),
		_thresholdScale(Gtk::ORIENTATION_HORIZONTAL),
		_clipDownButton("Clip values below rms as well as above"),
		_applyButton("Apply")
		{
			_box.pack_start(_thresholdLabel);

			_box.pack_start(_thresholdScale);
			_thresholdScale.set_range(0, 10);
			_thresholdScale.set_increments(0.1, 1.0);
			_thresholdScale.set_value(_action.Threshold());

			_box.pack_start(_clipDownButton);
			_clipDownButton.set_active(_action.ClipDown());
			_clipDownButton.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &FrequencySelectionFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::FrequencySelectionAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _thresholdLabel;
		Gtk::Scale _thresholdScale;
		Gtk::CheckButton _clipDownButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetThreshold(_thresholdScale.get_value());
			_action.SetClipDown(_clipDownButton.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // FREQUENCYSELECTIONFRAME_H
