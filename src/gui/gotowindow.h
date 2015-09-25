#ifndef GOTOWINDOW_H
#define GOTOWINDOW_H

#include <string>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>

#include "../strategy/control/types.h"

class GoToWindow : public Gtk::Window {
	public:
		GoToWindow(class RFIGuiWindow &rfiGuiWindow);
		~GoToWindow();
	private:
		void onLoadClicked();

		Gtk::ButtonBox _buttonBox;
		Gtk::VBox _vBox;
		Gtk::HBox _hBox;
		Gtk::VBox _bandFrameBox;
		Gtk::Frame _antenna1Frame, _antenna2Frame, _bandFrame, _sequenceFrame;
		Gtk::TreeView _antenna1View, _antenna2View, _bandView, _sequenceView;
		Gtk::ScrolledWindow _antenna1Scroll, _antenna2Scroll, _bandScroll, _sequenceScroll;
		Gtk::Button _loadButton;

		class AntennaModelColumns : public Gtk::TreeModelColumnRecord
		{
		public:
			AntennaModelColumns()
				{ add(antennaIndex); add(antennaName); }
		
			Gtk::TreeModelColumn<size_t> antennaIndex;
			Gtk::TreeModelColumn<Glib::ustring> antennaName;
		};

		class BandModelColumns : public Gtk::TreeModelColumnRecord
		{
		public:
			BandModelColumns()
				{ add(bandIndex); add(bandDescription); }
		
			Gtk::TreeModelColumn<size_t> bandIndex;
			Gtk::TreeModelColumn<Glib::ustring> bandDescription;
		};

		class SequenceModelColumns : public Gtk::TreeModelColumnRecord
		{
		public:
			SequenceModelColumns()
				{ add(sequenceIndex); add(sequenceDescription); }
		
			Gtk::TreeModelColumn<size_t> sequenceIndex;
			Gtk::TreeModelColumn<Glib::ustring> sequenceDescription;
		};

		AntennaModelColumns _antennaModelColumns;
		BandModelColumns _bandModelColumns;
		SequenceModelColumns _sequenceModelColumns;

		Glib::RefPtr<Gtk::ListStore> _antennaeStore, _bandStore, _sequenceStore;

		RFIGuiWindow &_rfiGuiWindow;
		rfiStrategy::MSImageSet *_imageSet;
};

#endif
