#include <gtkmm/button.h>
#include <gtkmm/image.h>

inline void gtkmm_set_image_from_icon_name(Gtk::Button& button, const Glib::ustring& icon_name, Gtk::IconSize size = Gtk::ICON_SIZE_BUTTON)
{
  Gtk::Image* image = Gtk::manage(new Gtk::Image());
  image->set_from_icon_name(icon_name, size);
  button.set_image(*image);
}
