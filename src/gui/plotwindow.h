#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <boost/bind/bind.hpp>

#include <gtkmm/box.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>
#include <gtkmm/toolbar.h>

#include "plot/plotwidget.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class PlotWindow : public Gtk::Window {
	public:
		PlotWindow(class PlotManager &plotManager);
		
		~PlotWindow();
		
	private:
		class PlotListColumns : public Gtk::TreeModel::ColumnRecord
		{
		public:
			PlotListColumns()
			{ add(_index); add(_name); }

			Gtk::TreeModelColumn<unsigned int> _index;
			Gtk::TreeModelColumn<Glib::ustring> _name;
		} _plotListColumns;
		
		void onSelectedPlotChange();
		void onClearPlotsPressed();
		void onEditPlottingPropertiesPressed();
		void onPlotPropertiesChanged();
		
		void handleUpdate();
		
		void updatePlotList();
		
		PlotWidget _plotWidget;
		class PlotManager &_plotManager;
		Gtk::HBox _hBox;
		Gtk::VBox _sideBox;
		Gtk::Toolbar _toolbar;
		Gtk::ToolButton _clearButton, _editButton;
		Glib::RefPtr<Gtk::ListStore> _plotListStore;
		Gtk::TreeView _plotListView;
		class PlotPropertiesWindow *_plotPropertiesWindow;
};

#endif
