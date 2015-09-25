#ifndef FREQUENCYCONVOLUTIONFRAME_H
#define FREQUENCYCONVOLUTIONFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/frequencyconvolutionaction.h"

#include "../editstrategywindow.h"

class FrequencyConvolutionFrame : public Gtk::Frame {
	public:
		FrequencyConvolutionFrame(rfiStrategy::FrequencyConvolutionAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Frequency convolution"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_rectangularKernelButton("Rectangular kernel"),
		_sincKernelButton("Sinc kernel"),
		_totalKernelButton("Total kernel"),
		_convolutionSizeLabel("Convolution size:"),
		_convolutionSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_inSamplesButton("Size in samples"),
		_applyButton("Apply")
		{
			Gtk::RadioButton::Group kernelGroup;
		
			_rectangularKernelButton.set_group(kernelGroup);
			_box.pack_start(_rectangularKernelButton);
			_sincKernelButton.set_group(kernelGroup);
			_box.pack_start(_sincKernelButton);
			_totalKernelButton.set_group(kernelGroup);
			_box.pack_start(_totalKernelButton);
			
			if(_action.KernelKind() == rfiStrategy::FrequencyConvolutionAction::RectangleKernel)
				_rectangularKernelButton.set_active(true);
			else if(_action.KernelKind() == rfiStrategy::FrequencyConvolutionAction::SincKernel)
				_sincKernelButton.set_active(true);
			else
				_totalKernelButton.set_active(true);

			_box.pack_start(_convolutionSizeLabel);
			
			_box.pack_start(_convolutionSizeScale);
			_convolutionSizeScale.set_range(1, 1024);
			_convolutionSizeScale.set_value(_action.ConvolutionSize());

			_box.pack_start(_inSamplesButton);
			_inSamplesButton.set_active(_action.InSamples());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &FrequencyConvolutionFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::FrequencyConvolutionAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::RadioButton _rectangularKernelButton, _sincKernelButton, _totalKernelButton;
		Gtk::Label _convolutionSizeLabel;
		Gtk::Scale _convolutionSizeScale;
		Gtk::CheckButton _inSamplesButton;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			if(_rectangularKernelButton.get_active())
				_action.SetKernelKind(rfiStrategy::FrequencyConvolutionAction::RectangleKernel);
			else if(_sincKernelButton.get_active())
				_action.SetKernelKind(rfiStrategy::FrequencyConvolutionAction::SincKernel);
			else if(_totalKernelButton.get_active())
				_action.SetKernelKind(rfiStrategy::FrequencyConvolutionAction::TotalKernel);
			_action.SetConvolutionSize(_convolutionSizeScale.get_value());
			_action.SetInSamples(_inSamplesButton.get_active());
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // FREQUENCYCONVOLUTIONFRAME_H
