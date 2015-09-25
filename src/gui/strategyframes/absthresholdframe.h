#ifndef ABSTHRESHOLDFRAME_H
#define ABSTHRESHOLDFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/absthresholdaction.h"

#include "../editstrategywindow.h"

class AbsThresholdFrame : public Gtk::Frame {
	public:
		AbsThresholdFrame(rfiStrategy::AbsThresholdAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Absolute threshold"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_thresholdLabel("Threshold:"),
		_applyButton("Apply")
		{
			_box.pack_start(_thresholdLabel);

			_box.pack_start(_thresholdEntry);
			std::stringstream s;
			s << _action.Threshold();
			_thresholdEntry.set_text(s.str());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &AbsThresholdFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::AbsThresholdAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _thresholdLabel;
		Gtk::Entry _thresholdEntry;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetThreshold(atof(_thresholdEntry.get_text().c_str()));
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // ABSTHRESHOLDFRAME_H
