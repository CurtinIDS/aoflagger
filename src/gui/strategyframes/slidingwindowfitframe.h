#ifndef SLIDINGWINDOWFITFRAME_H
#define SLIDINGWINDOWFITFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/slidingwindowfitaction.h"
#include "../../strategy/actions/slidingwindowfitparameters.h"

#include "../editstrategywindow.h"

class SlidingWindowFitFrame : public Gtk::Frame {
	public:
		SlidingWindowFitFrame(rfiStrategy::SlidingWindowFitAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Sliding window fit"), _editStrategyWindow(editStrategyWindow), _action(action),
		_hWindowSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_vWindowSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_hKernelSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_vKernelSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_hWindowSizeLabel("Horizontal sliding window size:", Gtk::ALIGN_START),
		_vWindowSizeLabel("Vertical sliding window size:", Gtk::ALIGN_START),
		_hKernelSizeLabel("Horizontal kernel size:", Gtk::ALIGN_START),
		_vKernelSizeLabel("Vertical kernel size:", Gtk::ALIGN_START),
		_iterationCountScale(Gtk::ORIENTATION_HORIZONTAL),
		_fitNoneButton("None"),
		_fitAverageButton("Average"),
		_fitWeightedAverageButton("W.Average"),
		_fitMedianButton("Median"),
		_fitMinimumButton("Minimum"),
		_applyButton("Apply")
		{
			initParameterWidgets();
		
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &SlidingWindowFitFrame::onApplyClicked));
			_buttonBox.pack_start(_applyButton);
			_applyButton.show();
		
			_buttonBox.show();
			_box.pack_end(_buttonBox);
		
			_box.show();
			add(_box);
		}

	private:
		void initParameterWidgets()
		{
			initMethodWidgets();
			initScales();
		}

		void initMethodWidgets()
		{
			Gtk::RadioButton::Group methodGroup;
		
			_fitNoneButton.set_group(methodGroup);
			_box.pack_start(_fitNoneButton);
		
			_fitAverageButton.set_group(methodGroup);
			_box.pack_start(_fitAverageButton);
		
			_fitWeightedAverageButton.set_group(methodGroup);
			_box.pack_start(_fitWeightedAverageButton);
		
			_fitMedianButton.set_group(methodGroup);
			_box.pack_start(_fitMedianButton);
		
			_fitMinimumButton.set_group(methodGroup);
			_box.pack_start(_fitMinimumButton);

			switch(_action.Parameters().method)
			{
				case rfiStrategy::SlidingWindowFitParameters::None:
					_fitNoneButton.set_active(true);
					break;
				case rfiStrategy::SlidingWindowFitParameters::Average:
					_fitAverageButton.set_active(true);
					break;
				case rfiStrategy::SlidingWindowFitParameters::GaussianWeightedAverage:
					_fitWeightedAverageButton.set_active(true);
					break;
				case rfiStrategy::SlidingWindowFitParameters::Median:
					_fitMedianButton.set_active(true);
					break;
				case rfiStrategy::SlidingWindowFitParameters::Minimum:
					_fitMinimumButton.set_active(true);
					break;
			}

			_fitNoneButton.show();
			_fitAverageButton.show();
			_fitWeightedAverageButton.show();
			_fitMedianButton.show();
			_fitMinimumButton.show();
		}

		void initScales()
		{
			_box.pack_start(_hWindowSizeLabel);
			_hWindowSizeLabel.show();
			_hWindowSizeScale.set_range(0.0, 400.0);
			_hWindowSizeScale.set_value(_action.Parameters().timeDirectionWindowSize);
			_box.pack_start(_hWindowSizeScale);
			_hWindowSizeScale.show();
		
			_box.pack_start(_vWindowSizeLabel);
			_vWindowSizeLabel.show();
			_vWindowSizeScale.set_range(0.0, 400.0);
			_vWindowSizeScale.set_value(_action.Parameters().frequencyDirectionWindowSize);
			_box.pack_start(_vWindowSizeScale);
			_vWindowSizeScale.show();
		
			_box.pack_start(_hKernelSizeLabel);
			_hKernelSizeLabel.show();
			_hKernelSizeScale.set_range(0.1, 1000.0);
			_hKernelSizeScale.set_increments(0.1, 1.0);
			_hKernelSizeScale.set_value(_action.Parameters().timeDirectionKernelSize);
			_box.pack_start(_hKernelSizeScale);
			_hKernelSizeScale.show();
		
			_box.pack_start(_vKernelSizeLabel);
			_vKernelSizeLabel.show();
			_vKernelSizeScale.set_range(0.1, 1000.0);
			_vKernelSizeScale.set_increments(0.1, 1.0);
			_vKernelSizeScale.set_value(_action.Parameters().frequencyDirectionKernelSize);
			_box.pack_start(_vKernelSizeScale);
			_vKernelSizeScale.show();
		}

		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::SlidingWindowFitAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Scale
			_hWindowSizeScale, _vWindowSizeScale,
			_hKernelSizeScale, _vKernelSizeScale;
		Gtk::Label
			_hWindowSizeLabel, _vWindowSizeLabel,
			_hKernelSizeLabel, _vKernelSizeLabel;
		Gtk::Scale _iterationCountScale;
		Gtk::RadioButton
			_fitNoneButton, _fitAverageButton, _fitWeightedAverageButton, _fitMedianButton, _fitMinimumButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			enum rfiStrategy::SlidingWindowFitParameters::Method method;
			if(_fitAverageButton.get_active())
				method = rfiStrategy::SlidingWindowFitParameters::Average;
			else if(_fitWeightedAverageButton.get_active())
				method = rfiStrategy::SlidingWindowFitParameters::GaussianWeightedAverage;
			else if(_fitMedianButton.get_active())
				method = rfiStrategy::SlidingWindowFitParameters::Median;
			else if(_fitMinimumButton.get_active())
				method = rfiStrategy::SlidingWindowFitParameters::Minimum;
			else
				method = rfiStrategy::SlidingWindowFitParameters::None;
			_action.Parameters().method = method;
			_action.Parameters().timeDirectionWindowSize = (size_t) _hWindowSizeScale.get_value();
			_action.Parameters().frequencyDirectionWindowSize = (size_t) _vWindowSizeScale.get_value();
			_action.Parameters().timeDirectionKernelSize = _hKernelSizeScale.get_value();
			_action.Parameters().frequencyDirectionKernelSize = _vKernelSizeScale.get_value();

			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // SLIDINGWINDOWFITFRAME_H
