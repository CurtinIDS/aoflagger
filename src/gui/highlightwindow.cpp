#include "highlightwindow.h"

#include "../strategy/algorithms/thresholdconfig.h"

#include "rfiguiwindow.h"

#include <iostream>

HighlightWindow::HighlightWindow(RFIGuiWindow &rfiGuiWindow) :
	_rfiGuiWindow(rfiGuiWindow),
	_highlightThresholdHighScale(Gtk::ORIENTATION_VERTICAL),
	_highlightThresholdLowScale(Gtk::ORIENTATION_VERTICAL),
	_connectedCountScale(Gtk::ORIENTATION_HORIZONTAL),
	_highlightButton("Highlighting")
{
	_highlightThresholdHighScale.set_inverted(true);
	_highlightThresholdHighScale.set_range(0.0, 1.0);
	_highlightThresholdHighScale.set_increments(0.001, 0.01);
	_highlightThresholdHighScale.set_value(1.0);
	_highlightThresholdHighScale.set_size_request(-1, 200);
	_highlightThresholdHighScale.signal_value_changed().
		connect(sigc::mem_fun(*this, &HighlightWindow::onValueChange));
	_highlightThresholdBox.pack_start(_highlightThresholdHighScale);
	_highlightThresholdHighScale.show();

	_highlightThresholdLowScale.set_inverted(true);
	_highlightThresholdLowScale.set_range(-0.005, 0.005);
	_highlightThresholdLowScale.set_increments(0.00001, 0.0001);
	_highlightThresholdLowScale.set_value(0.0);
	_highlightThresholdLowScale.set_size_request(-1, 200);
	_highlightThresholdLowScale.signal_value_changed().
		connect(sigc::mem_fun(*this, &HighlightWindow::onValueChange));
	_highlightThresholdBox.pack_start(_highlightThresholdLowScale);
	_highlightThresholdLowScale.show();

	_mainBox.pack_start(_highlightThresholdBox);
	_highlightThresholdBox.show();

	_connectedCountScale.set_value(1.0);
	_connectedCountScale.set_range(1.0, 64.0);
	_connectedCountScale.signal_value_changed().
		connect(sigc::mem_fun(*this, &HighlightWindow::onValueChange));
	_mainBox.pack_start(_connectedCountScale);
	_connectedCountScale.show();

	_highlightButton.set_active(false);
	_highlightButton.signal_clicked().
		connect(sigc::mem_fun(*this, &HighlightWindow::onHighlightingToggled));
	_mainBox.pack_start(_highlightButton, false, true);
	_highlightButton.show();

	_max = _rfiGuiWindow.GetActiveData().GetSingleImage()->GetMaximum();

	add(_mainBox);
	_mainBox.show();
}


HighlightWindow::~HighlightWindow()
{
}

void HighlightWindow::onValueChange()
{
	// make val between [0,10]
	long double val =
		(_highlightThresholdHighScale.get_value() + _highlightThresholdLowScale.get_value()) *12.0L;
	// make it exponential and between [0,1]
	val = powl(2.0L, val) / (4096.0L);

	_rfiGuiWindow.HighlightConfig().InitializeThresholdsFromFirstThreshold(val*_max, ThresholdConfig::Gaussian);
	_rfiGuiWindow.HighlightConfig().SetMinimumConnectedSamples((size_t) round(_connectedCountScale.get_value()));
	if(_highlightButton.get_active())
		_rfiGuiWindow.Update();
	else
		_highlightButton.set_active(true);
}

void HighlightWindow::onHighlightingToggled()
{
	_rfiGuiWindow.SetHighlighting(_highlightButton.get_active());
	_rfiGuiWindow.Update();
}
