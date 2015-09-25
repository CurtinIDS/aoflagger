#ifndef SVDFRAME_H
#define SVDFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/svdaction.h"

#include "../editstrategywindow.h"

class SVDFrame : public Gtk::Frame {
	public:
		SVDFrame(rfiStrategy::SVDAction &svdAction, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Singular value decomposition"),
		_editStrategyWindow(editStrategyWindow), _svdAction(svdAction),
		_singularValueCountLabel("Singular value count:"),
		_singularValueCountScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			_box.pack_start(_singularValueCountLabel);
			_singularValueCountLabel.show();

			_box.pack_start(_singularValueCountScale);
			_singularValueCountScale.set_range(0, 100);
			_singularValueCountScale.set_value(_svdAction.SingularValueCount());
			_singularValueCountScale.show();

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &SVDFrame::onApplyClicked));
			_applyButton.show();

			_box.pack_start(_buttonBox);
			_buttonBox.show();

			add(_box);
			_box.show();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::SVDAction &_svdAction;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _singularValueCountLabel;
		Gtk::Scale _singularValueCountScale;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_svdAction.SetSingularValueCount((size_t) _singularValueCountScale.get_value());
			_editStrategyWindow.UpdateAction(&_svdAction);
		}
};

#endif // SVDFRAME_H
