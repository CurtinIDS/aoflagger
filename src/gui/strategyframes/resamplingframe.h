#ifndef RESAMPLINGFRAME_H
#define RESAMPLINGFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/resamplingaction.h"

#include "../editstrategywindow.h"

class ResamplingFrame : public Gtk::Frame {
	public:
		ResamplingFrame(rfiStrategy::ResamplingAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Resampling"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_averagingButton("Average"),
		_nnButton("Nearest neighbour"),
		_sizeXLabel("Size x:"),
		_sizeYLabel("Size y"),
		_sizeXScale(Gtk::ORIENTATION_HORIZONTAL),
		_sizeYScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			Gtk::RadioButton::Group group;
			_averagingButton.set_group(group);
			_box.pack_start(_averagingButton);
			
			_nnButton.set_group(group);
			_box.pack_start(_nnButton);
			if(_action.Operation() == rfiStrategy::ResamplingAction::Average)
				_averagingButton.set_active(true);
			else
				_nnButton.set_active(true);
			
			_box.pack_start(_sizeXLabel);

			_box.pack_start(_sizeXScale);
			_sizeXScale.set_range(1, 1024);
			_sizeXScale.set_value(_action.SizeX());

			_box.pack_start(_sizeYLabel);

			_box.pack_start(_sizeYScale);
			_sizeYScale.set_range(1, 1024);
			_sizeYScale.set_value(_action.SizeY());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &ResamplingFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::ResamplingAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::RadioButton _averagingButton, _nnButton;
		Gtk::Label _sizeXLabel, _sizeYLabel;
		Gtk::Scale _sizeXScale, _sizeYScale;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			if(_averagingButton.get_active())
				_action.SetOperation(rfiStrategy::ResamplingAction::Average);
			else
				_action.SetOperation(rfiStrategy::ResamplingAction::NearestNeighbour);
			_action.SetSizeX((unsigned) _sizeXScale.get_value());
			_action.SetSizeY((unsigned)_sizeYScale.get_value());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // RESAMPLINGFRAME_H
