#ifndef GUI_QUALITY__DATA_WINDOW_H
#define GUI_QUALITY__DATA_WINDOW_H

#include <string>

#include <gtkmm/box.h>
#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/window.h>

class DataWindow : public Gtk::Window {
	public:
		DataWindow()
		{
			_box.pack_start(_comboBox, Gtk::PACK_SHRINK);
			_comboListStore = Gtk::ListStore::create(_comboColumnRecord);
			_comboBox.set_model(_comboListStore);
			_comboBox.pack_start(_comboColumnRecord._comboListNameColumn);
			_comboBox.signal_changed().connect(sigc::mem_fun(*this, &DataWindow::onComboChange));
			_comboBox.show();
			
			_scrolledWindow.add(_textView);
			_textView.show();
			
			_box.pack_end(_scrolledWindow);
			_scrolledWindow.show();
			
			add(_box);
			_box.show();
			
			set_default_size(300, 400);
		}
    ~DataWindow()
    {
		}
		void SetData(const std::string &data)
		{
			_textView.get_buffer()->set_text(data);
		}
		void SetData(const class Plot2D &plot);
	private:
		DataWindow(const DataWindow &dataWindow) { }
		
		void onComboChange();
		void loadData(size_t plotSetIndex);
		
		class ComboColumnRecord : public Gtk::TreeModel::ColumnRecord
		{
		public:
			ComboColumnRecord() { add(_comboListNameColumn); }
			Gtk::TreeModelColumn<Glib::ustring> _comboListNameColumn;
		} _comboColumnRecord;
		Gtk::VBox _box;
		Gtk::ComboBox _comboBox;
		Glib::RefPtr<Gtk::ListStore> _comboListStore;
		Gtk::ScrolledWindow _scrolledWindow;
		Gtk::TextView _textView;
		const class Plot2D *_plot;
};

#endif
