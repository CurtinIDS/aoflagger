#ifndef TIMESELECTIONFRAME_H
#define TIMESELECTIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/timeselectionaction.h"

#include "../editstrategywindow.h"

class TimeSelectionFrame : public Gtk::Frame {
	public:
		TimeSelectionFrame(rfiStrategy::TimeSelectionAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Time selection"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_thresholdLabel("Threshold:"),
		_thresholdScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			_box.pack_start(_thresholdLabel);

			_box.pack_start(_thresholdScale);
			_thresholdScale.set_range(0, 10);
			_thresholdScale.set_increments(0.1, 1.0);
			_thresholdScale.set_value(_action.Threshold());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &TimeSelectionFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::TimeSelectionAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _thresholdLabel;
		Gtk::Scale _thresholdScale;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetThreshold(_thresholdScale.get_value());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // TIMESELECTIONFRAME_H
