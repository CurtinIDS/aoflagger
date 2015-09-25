#ifndef HIGHLIGHTWINDOW_H
#define HIGHLIGHTWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/scale.h>
#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>

#include "../msio/types.h"

class HighlightWindow : public Gtk::Window {
	public:
		HighlightWindow(class RFIGuiWindow &rfiGuiWindow);
		~HighlightWindow();
	private:
		void onValueChange();
		void onHighlightingToggled();

		class RFIGuiWindow &_rfiGuiWindow;
		Gtk::Scale _highlightThresholdHighScale;
		Gtk::Scale _highlightThresholdLowScale;
		Gtk::Scale _connectedCountScale;
		Gtk::VBox _mainBox;
		Gtk::HBox _highlightThresholdBox;
		Gtk::CheckButton _highlightButton;

		num_t _max;
};

#endif
