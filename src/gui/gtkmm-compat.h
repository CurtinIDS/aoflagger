#include <gtkmm/button.h>
#include <gtkmm/image.h>

void set_image_from_icon_name(const Glib::ustring& icon_name, IconSize size)
{
  Image* image = Gtk::manage(new Image());
  image->set_from_icon_name(icon_name, size);
  set_image(*image);
}
