#ifndef STATISTICALFLAGGINGFRAME_H
#define STATISTICALFLAGGINGFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/statisticalflagaction.h"

#include "../editstrategywindow.h"

class StatisticalFlaggingFrame : public Gtk::Frame {
	public:
		StatisticalFlaggingFrame(rfiStrategy::StatisticalFlagAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Statistical flagging"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_dilluteTimeSizeLabel("Dillution time size:"),
		_dilluteTimeSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_dilluteFrequencySizeLabel("Dillution frequency size:"),
		_dilluteFrequencySizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_minTimeRatioLabel("Minimum time ratio:"),
		_minTimeRatioScale(Gtk::ORIENTATION_HORIZONTAL),
		_minFreqRatioLabel("Minimum frequency ratio:"),
		_minFreqRatioScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			_box.pack_start(_dilluteTimeSizeLabel);

			_dilluteTimeSizeScale.set_range(0, 100);
			_dilluteTimeSizeScale.set_value(_action.EnlargeTimeSize());
			_dilluteTimeSizeScale.set_increments(1, 10);
			_box.pack_start(_dilluteTimeSizeScale);

			_box.pack_start(_dilluteFrequencySizeLabel);

			_dilluteFrequencySizeScale.set_range(0, 100);
			_dilluteFrequencySizeScale.set_value(_action.EnlargeFrequencySize());
			_dilluteFrequencySizeScale.set_increments(1, 10);
			_box.pack_start(_dilluteFrequencySizeScale);

			_box.pack_start(_minTimeRatioLabel);

			_minTimeRatioScale.set_range(0, 100);
			_minTimeRatioScale.set_value(_action.MinimumGoodTimeRatio()*100.0);
			_minTimeRatioScale.set_increments(.25, 5);
			_box.pack_start(_minTimeRatioScale);

			_box.pack_start(_minFreqRatioLabel);

			_minFreqRatioScale.set_range(0, 100);
			_minFreqRatioScale.set_value(_action.MinimumGoodFrequencyRatio()*100.0);
			_minFreqRatioScale.set_increments(.25, 5);
			_box.pack_start(_minFreqRatioScale);

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &StatisticalFlaggingFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::StatisticalFlagAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _dilluteTimeSizeLabel;
		Gtk::Scale _dilluteTimeSizeScale;
		Gtk::Label _dilluteFrequencySizeLabel;
		Gtk::Scale _dilluteFrequencySizeScale;
		
		Gtk::Label _minTimeRatioLabel;
		Gtk::Scale _minTimeRatioScale;
		Gtk::Label _minFreqRatioLabel;
		Gtk::Scale _minFreqRatioScale;
		
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetEnlargeTimeSize((size_t) _dilluteTimeSizeScale.get_value());
			_action.SetEnlargeFrequencySize((size_t) _dilluteFrequencySizeScale.get_value());
			_action.SetMinimumGoodTimeRatio(_minTimeRatioScale.get_value()/100.0);
			_action.SetMinimumGoodFrequencyRatio(_minFreqRatioScale.get_value()/100.0);
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // STATISTICALFLAGGINGFRAME_H
