#ifndef THRESHOLDFRAME_H
#define THRESHOLDFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/sumthresholdaction.h"

#include "../editstrategywindow.h"

class SumThresholdFrame : public Gtk::Frame {
	public:
		SumThresholdFrame(rfiStrategy::SumThresholdAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("SumThreshold"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_sensitivityLabel("Base sensitivity: (low = sensitive)"),
		_sensitivityScale(Gtk::ORIENTATION_HORIZONTAL),
		_timeDirectionButton("In time direction"),
		_frequencyDirectionButton("In frequency direction"),
		_applyButton("Apply")
		{
			_box.pack_start(_sensitivityLabel);
			_sensitivityLabel.show();

			_box.pack_start(_sensitivityScale);
			_sensitivityScale.set_range(0, 10);
			_sensitivityScale.set_increments(0.1, 1.0);
			_sensitivityScale.set_value(_action.BaseSensitivity());
			_sensitivityScale.show();
			
			_timeDirectionButton.set_active(_action.TimeDirectionFlagging());
			_buttonBox.pack_start(_timeDirectionButton);
			_timeDirectionButton.show();

			_frequencyDirectionButton.set_active(_action.FrequencyDirectionFlagging());
			_buttonBox.pack_start(_frequencyDirectionButton);
			_frequencyDirectionButton.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &SumThresholdFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::SumThresholdAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _sensitivityLabel;
		Gtk::Scale _sensitivityScale;
		Gtk::CheckButton _timeDirectionButton, _frequencyDirectionButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetBaseSensitivity(_sensitivityScale.get_value());
			_action.SetTimeDirectionFlagging(_timeDirectionButton.get_active());
			_action.SetFrequencyDirectionFlagging(_frequencyDirectionButton.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // THRESHOLDFRAME_H
