#ifndef SPATIALCOMPOSITIONFRAME_H
#define SPATIALCOMPOSITIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>

#include "../../strategy/actions/spatialcompositionaction.h"

#include "../editstrategywindow.h"

class SpatialCompositionFrame : public Gtk::Frame {
	public:
		SpatialCompositionFrame(rfiStrategy::SpatialCompositionAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Set image"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_sumCrossButton("Sum cross correlations"),
		_sumAutoButton("Sum auto correlations"),
		_eigenvalueButton("First eigenvalues of complete matrix"),
		_removeEigenvalueButton("Remove first eigenvalues"),
		_applyButton("Apply")
		{
			Gtk::RadioButton::Group group;

			_box.pack_start(_sumCrossButton);
			_sumCrossButton.set_group(group);

			_box.pack_start(_sumAutoButton);
			_sumAutoButton.set_group(group);

			_box.pack_start(_eigenvalueButton);
			_eigenvalueButton.set_group(group);
			
			_box.pack_start(_removeEigenvalueButton);
			_removeEigenvalueButton.set_group(group);
			
			switch(_action.Operation())
			{
				case rfiStrategy::SpatialCompositionAction::SumCrossCorrelationsOperation:
				_sumCrossButton.set_active(true);
					break;
				case rfiStrategy::SpatialCompositionAction::SumAutoCorrelationsOperation:
				_sumAutoButton.set_active(true);
					break;
				case rfiStrategy::SpatialCompositionAction::EigenvalueDecompositionOperation:
				_eigenvalueButton.set_active(true);
					break;
				case rfiStrategy::SpatialCompositionAction::EigenvalueRemovalOperation:
				_removeEigenvalueButton.set_active(true);
					break;
			}

			_sumCrossButton.show();
			_sumAutoButton.show();
			_eigenvalueButton.show();
			_removeEigenvalueButton.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &SpatialCompositionFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::SpatialCompositionAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _baselinesLabel;
		Gtk::RadioButton
			_sumCrossButton, _sumAutoButton, _eigenvalueButton, _removeEigenvalueButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			if(_sumCrossButton.get_active())
				_action.SetOperation(rfiStrategy::SpatialCompositionAction::SumCrossCorrelationsOperation);
			else if(_sumAutoButton.get_active())
				_action.SetOperation(rfiStrategy::SpatialCompositionAction::SumAutoCorrelationsOperation);
			else if(_eigenvalueButton.get_active())
				_action.SetOperation(rfiStrategy::SpatialCompositionAction::EigenvalueDecompositionOperation);
			else if(_removeEigenvalueButton.get_active())
				_action.SetOperation(rfiStrategy::SpatialCompositionAction::EigenvalueRemovalOperation);
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // SPATIALCOMPOSITIONFRAME_H
