#ifndef CHANGERESOLUTIONFRAME_H
#define CHANGERESOLUTIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/changeresolutionaction.h"

#include "../editstrategywindow.h"

class ChangeResolutionFrame : public Gtk::Frame {
	public:
		ChangeResolutionFrame(rfiStrategy::ChangeResolutionAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Change resolution"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_timeDecreaseFactorLabel("Time decrease factor:"),
		_timeDecreaseFactorScale(Gtk::ORIENTATION_HORIZONTAL),
		_frequencyDecreaseFactorLabel("Frequency decrease factor:"),
		_frequencyDecreaseFactorScale(Gtk::ORIENTATION_HORIZONTAL),
		_setRevisedToChangedImage("Set revised images to changed image"),
		_setContaminatedToChangedImage("Set contaminated images to changed image"),
		_setMasksToChangedMasks("Set masks to changed masks"),
		_useMaskForAveraging("Use mask for averaging"),
		_applyButton("Apply")
		{
			_box.pack_start(_timeDecreaseFactorLabel);

			_timeDecreaseFactorScale.set_range(1, 128);
			_timeDecreaseFactorScale.set_increments(1, 5);
			_box.pack_start(_timeDecreaseFactorScale);
			_timeDecreaseFactorScale.set_value(_action.TimeDecreaseFactor());

			_box.pack_start(_frequencyDecreaseFactorLabel);

			_frequencyDecreaseFactorScale.set_range(1, 256);
			_frequencyDecreaseFactorScale.set_increments(1, 5);
			_box.pack_start(_frequencyDecreaseFactorScale);
			_frequencyDecreaseFactorScale.set_value(_action.FrequencyDecreaseFactor());

			_box.pack_start(_setRevisedToChangedImage);
			_setRevisedToChangedImage.set_active(_action.RestoreRevised());

			_box.pack_start(_setContaminatedToChangedImage);
			_setContaminatedToChangedImage.set_active(_action.RestoreContaminated());

			_box.pack_start(_setMasksToChangedMasks);
			_setMasksToChangedMasks.set_active(_action.RestoreMasks());

			_box.pack_start(_useMaskForAveraging);
			_useMaskForAveraging.set_active(_action.UseMaskInAveraging());
			
			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &ChangeResolutionFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::ChangeResolutionAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _timeDecreaseFactorLabel;
		Gtk::Scale _timeDecreaseFactorScale;
		Gtk::Label _frequencyDecreaseFactorLabel;
		Gtk::Scale _frequencyDecreaseFactorScale;
		Gtk::CheckButton _setRevisedToChangedImage;
		Gtk::CheckButton _setContaminatedToChangedImage;
		Gtk::CheckButton _setMasksToChangedMasks;
		Gtk::CheckButton _useMaskForAveraging;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetTimeDecreaseFactor((size_t) _timeDecreaseFactorScale.get_value());
			_action.SetFrequencyDecreaseFactor((size_t) _frequencyDecreaseFactorScale.get_value());
			_action.SetRestoreRevised(_setRevisedToChangedImage.get_active());
			_action.SetRestoreContaminated(_setContaminatedToChangedImage.get_active());
			_action.SetRestoreMasks(_setMasksToChangedMasks.get_active());
			_action.SetUseMaskInAveraging(_useMaskForAveraging.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // CHANGERESOLUTIONFRAME_H
