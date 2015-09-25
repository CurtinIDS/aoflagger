#ifndef BASELINESELECTIONFRAME_H
#define BASELINESELECTIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>

#include "../../strategy/actions/baselineselectionaction.h"

#include "../editstrategywindow.h"

class BaselineSelectionFrame : public Gtk::Frame {
	public:
		BaselineSelectionFrame(rfiStrategy::BaselineSelectionAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Baseline selection"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_preparationStepButton("This is the preparation step"),
		_flagBadBaselinesButton("Flag bad baselines"),
		_makePlotButton("Make a length-rfi plot"),
		_applyButton("Apply")
		{
			_box.pack_start(_preparationStepButton);
			_preparationStepButton.set_active(_action.PreparationStep());
			_preparationStepButton.show();

			_box.pack_start(_flagBadBaselinesButton);
			_flagBadBaselinesButton.set_active(_action.FlagBadBaselines());
			_flagBadBaselinesButton.show();

			_box.pack_start(_makePlotButton);
			_makePlotButton.set_active(_action.MakePlot());
			_makePlotButton.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &BaselineSelectionFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::BaselineSelectionAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::CheckButton _preparationStepButton, _flagBadBaselinesButton, _makePlotButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetPreparationStep(_preparationStepButton.get_active());
			_action.SetFlagBadBaselines(_flagBadBaselinesButton.get_active());
			_action.SetMakePlot(_makePlotButton.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // BASELINESELECTIONFRAME_H
