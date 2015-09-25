#ifndef NUMINPUTDIALOG_H
#define NUMINPUTDIALOG_H

#include <cstdlib>
#include <sstream>

#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>

class NumInputDialog : public Gtk::Dialog
{
	public:
		NumInputDialog(const Glib::ustring& title, const Glib::ustring& valueCaption, double defaultValue)
			: Dialog(title, true), _label(valueCaption)
		{
			_hBox.pack_start(_label);
			
			std::ostringstream s;
			s << defaultValue;
			_entry.set_text(s.str());
			_entry.set_activates_default(true);
			_hBox.pack_end(_entry);
			
			get_content_area()->pack_start(_hBox);
			_hBox.show_all();
			
			add_button("_Ok", Gtk::RESPONSE_OK);
			add_button("_Cancel", Gtk::RESPONSE_CANCEL);
			set_default_response(Gtk::RESPONSE_OK);
		}
		
		double Value() const
		{
			return atof(_entry.get_text().c_str());
		}
	private:
		Gtk::Label _label;
		Gtk::Entry _entry;
		Gtk::HBox _hBox;
};

#endif // NUMINPUTDIALOG_H
