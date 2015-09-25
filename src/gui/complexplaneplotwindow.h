#ifndef COMPLEXPLANEPLOTWINDOW_H
#define COMPLEXPLANEPLOTWINDOW_H

#include <vector>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/scale.h>
#include <gtkmm/window.h>

class ComplexPlanePlotWindow : public Gtk::Window {
	public:
		ComplexPlanePlotWindow(class RFIGuiWindow &rfiGuiWindow, class PlotManager &plotManager);
		~ComplexPlanePlotWindow();
	private:
		size_t XStart() const throw() { return (size_t) _xPositionScale.get_value(); }
		size_t XLength() const throw() { return (size_t) _lengthScale.get_value(); }

		size_t YStart() const throw() { return (size_t) _yPositionScale.get_value(); }
		size_t YLength() const throw() { return (size_t) _ySumLengthScale.get_value(); }

		void onPlotPressed();
		void onTimeStartChanged() {
			if(XStart() + XLength() > _xMax)
				_lengthScale.set_value(_xMax - XStart());
			else
				setDetailsLabel();
		}
		void onTimeDurationChanged() {
			if(XStart() + XLength() > _xMax)
				_xPositionScale.set_value(_xMax - XLength());
			else
				setDetailsLabel();
		}
		void onFreqChanged() {
			if(YStart() + YLength() > _yMax)
				_ySumLengthScale.set_value(_yMax - YStart());
			else
				setDetailsLabel();
		}
		void onFreqSizeChanged() {
			if(YStart() + YLength() > _yMax)
				_yPositionScale.set_value(_yMax - YLength());
			else
				setDetailsLabel();
		}
		void setDetailsLabel();

		class RFIGuiWindow &_rfiGuiWindow;
		class PlotManager &_plotManager;
		Gtk::Frame _detailsFrame;
		Gtk::VBox _mainBox, _detailsBox;
		Gtk::Label
			_detailsLabel,
			_xPositionLabel, _yPositionLabel,
			_lengthLabel, _ySumLengthLabel;
		Gtk::Scale
			_xPositionScale, _yPositionScale, 
			_lengthScale, _ySumLengthScale;
		Gtk::RadioButton _realVersusImaginaryButton, _timeVersusRealButton;
		Gtk::CheckButton _allValuesButton, _unmaskedValuesButton, _maskedValuesButton, _fittedValuesButton, _individualSampleFitButton, _fringeFitButton, _dynamicFringeFitButton;

		Gtk::Button _plotButton;
		Gtk::ButtonBox _buttonBox;
		std::vector<double> _observationTimes;
		size_t _xMax, _yMax;
};

#endif
