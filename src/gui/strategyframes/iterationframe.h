#ifndef ITERATIONFRAME_H
#define ITERATIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/iterationaction.h"

#include "../editstrategywindow.h"

class IterationFrame : public Gtk::Frame {
	public:
		IterationFrame(rfiStrategy::IterationBlock &iterationBlock, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Iteration"),
		_editStrategyWindow(editStrategyWindow), _iterationBlock(iterationBlock),
		_iterationCountLabel("Iteration count:"),
		_sensitivityStartLabel("Sensitivity start value (moves to 1):"),
		_iterationCountScale(Gtk::ORIENTATION_HORIZONTAL),
		_sensitivityStartScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			_box.pack_start(_iterationCountLabel);

			_box.pack_start(_iterationCountScale);
			_iterationCountScale.set_range(0, 200);
			_iterationCountScale.set_value(_iterationBlock.IterationCount());
			_iterationCountScale.set_increments(1, 10);

			_box.pack_start(_sensitivityStartLabel);

			_box.pack_start(_sensitivityStartScale);
			_sensitivityStartScale.set_range(0, 25.0);
			_sensitivityStartScale.set_increments(1, 5);
			_sensitivityStartScale.set_value(_iterationBlock.SensitivityStart());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &IterationFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::IterationBlock &_iterationBlock;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _iterationCountLabel, _sensitivityStartLabel;
		Gtk::Scale _iterationCountScale, _sensitivityStartScale;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_iterationBlock.SetIterationCount((size_t) _iterationCountScale.get_value());
			_iterationBlock.SetSensitivityStart(_sensitivityStartScale.get_value());
			_editStrategyWindow.UpdateAction(&_iterationBlock);

		}
};

#endif // ITERATIONFRAME_H
