#ifndef CUTAREAFRAME_H
#define CUTAREAFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>

#include "../../strategy/actions/cutareaaction.h"

#include "../editstrategywindow.h"

class CutAreaFrame : public Gtk::Frame {
	public:
		CutAreaFrame(rfiStrategy::CutAreaAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Cut area"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_startTimeLabel("Start time steps to cut:"),
		_startTimeScale(Gtk::ORIENTATION_HORIZONTAL),
		_endTimeLabel("End time steps to cut:"),
		_endTimeScale(Gtk::ORIENTATION_HORIZONTAL),
		_topChannelsLabel("Top channels to cut:"),
		_topChannelsScale(Gtk::ORIENTATION_HORIZONTAL),
		_bottomChannelsLabel("Bottom channels to cut:"),
		_bottomChannelsScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			_box.pack_start(_startTimeLabel);
			_startTimeScale.set_range(0, 64);
			_box.pack_start(_startTimeScale);
			_startTimeScale.set_value(_action.StartTimeSteps());

			_box.pack_start(_endTimeLabel);
			_endTimeScale.set_range(0, 64);
			_box.pack_start(_endTimeScale);
			_endTimeScale.set_value(_action.EndTimeSteps());

			_box.pack_start(_topChannelsLabel);
			_topChannelsScale.set_range(0, 64);
			_box.pack_start(_topChannelsScale);
			_topChannelsScale.set_value(_action.TopChannels());

			_box.pack_start(_bottomChannelsLabel);
			_bottomChannelsScale.set_range(0, 64);
			_box.pack_start(_bottomChannelsScale);
			_bottomChannelsScale.set_value(_action.BottomChannels());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &CutAreaFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::CutAreaAction &_action;

		Gtk::Label _startTimeLabel;
		Gtk::Scale _startTimeScale;
		Gtk::Label _endTimeLabel;
		Gtk::Scale _endTimeScale;
		Gtk::Label _topChannelsLabel;
		Gtk::Scale _topChannelsScale;
		Gtk::Label _bottomChannelsLabel;
		Gtk::Scale _bottomChannelsScale;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetStartTimeSteps((int) _startTimeScale.get_value());
			_action.SetEndTimeSteps((int) _endTimeScale.get_value());
			_action.SetTopChannels((int) _topChannelsScale.get_value());
			_action.SetBottomChannels((int) _bottomChannelsScale.get_value());

			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // CUTAREAFRAME_H
