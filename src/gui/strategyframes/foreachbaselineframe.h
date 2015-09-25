#ifndef FOREACHBASELINEFRAME_H
#define FOREACHBASELINEFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/foreachbaselineaction.h"

#include "../editstrategywindow.h"

class ForEachBaselineFrame : public Gtk::Frame {
	public:
		ForEachBaselineFrame(rfiStrategy::ForEachBaselineAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("For each baseline"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_baselinesLabel("Baselines to iterate over:"),
		_allBaselinesButton("All"),
		_crossBaselinesButton("Cross-correlated"),
		_autoBaselinesButton("Auto-correlated"),
		_equalToCurrentBaselinesButton("Equal to current"),
		_autoOfCurrentBaselinesButton("Auto of current antennae"),
		_currentBaselineButton("Current"),
		_threadCountLabel("Thread count:"),
		_threadCountScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("A_pply")
		{
			_box.pack_start(_baselinesLabel);
			_baselinesLabel.show();

			Gtk::RadioButton::Group group;

			_box.pack_start(_allBaselinesButton);
			_allBaselinesButton.set_group(group);

			_box.pack_start(_crossBaselinesButton);
			_crossBaselinesButton.set_group(group);

			_box.pack_start(_autoBaselinesButton);
			_autoBaselinesButton.set_group(group);

			_box.pack_start(_equalToCurrentBaselinesButton);
			_equalToCurrentBaselinesButton.set_group(group);

			_box.pack_start(_autoOfCurrentBaselinesButton);
			_autoOfCurrentBaselinesButton.set_group(group);

			_box.pack_start(_currentBaselineButton);
			_currentBaselineButton.set_group(group);

			switch(_action.Selection())
			{
				case rfiStrategy::All:
				_allBaselinesButton.set_active(true);
					break;
				case rfiStrategy::CrossCorrelations:
				_crossBaselinesButton.set_active(true);
					break;
				case rfiStrategy::AutoCorrelations:
				_autoBaselinesButton.set_active(true);
					break;
				case rfiStrategy::EqualToCurrent:
				_equalToCurrentBaselinesButton.set_active(true);
					break;
				case rfiStrategy::AutoCorrelationsOfCurrentAntennae:
				_autoOfCurrentBaselinesButton.set_active(true);
					break;
				case rfiStrategy::Current:
				_currentBaselineButton.set_active(true);
					break;
			}

			_allBaselinesButton.show();
			_crossBaselinesButton.show();
			_autoBaselinesButton.show();
			_equalToCurrentBaselinesButton.show();
			_autoOfCurrentBaselinesButton.show();
			_currentBaselineButton.show();

			_box.pack_start(_threadCountLabel);
			_threadCountLabel.show();

			_threadCountScale.set_range(1, 32);
			_threadCountScale.set_value(action.ThreadCount());
			_box.pack_start(_threadCountScale);
			_threadCountScale.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &ForEachBaselineFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::ForEachBaselineAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _baselinesLabel;
		Gtk::RadioButton
			_allBaselinesButton, _crossBaselinesButton, _autoBaselinesButton, _equalToCurrentBaselinesButton, _autoOfCurrentBaselinesButton, _currentBaselineButton;
		Gtk::Label _threadCountLabel;
		Gtk::Scale _threadCountScale;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			if(_allBaselinesButton.get_active())
				_action.SetSelection(rfiStrategy::All);
			else if(_crossBaselinesButton.get_active())
				_action.SetSelection(rfiStrategy::CrossCorrelations);
			else if(_autoBaselinesButton.get_active())
				_action.SetSelection(rfiStrategy::AutoCorrelations);
			else if(_equalToCurrentBaselinesButton.get_active())
				_action.SetSelection(rfiStrategy::EqualToCurrent);
			else if(_autoOfCurrentBaselinesButton.get_active())
				_action.SetSelection(rfiStrategy::AutoCorrelationsOfCurrentAntennae);
			else if(_currentBaselineButton.get_active())
				_action.SetSelection(rfiStrategy::Current);
			_action.SetThreadCount((int) _threadCountScale.get_value());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // FOREACHBASELINEFRAME_H
