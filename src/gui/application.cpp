#include "application.h"

#include <gtkmm/application.h>

#include "rfiguiwindow.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run(int argc, char *argv[])
{
	// We have to 'lie' about argc to create(..), because of a bug in older gtkmms.
	int altArgc = 1;
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(altArgc, argv, "", Gio::APPLICATION_HANDLES_OPEN);
	RFIGuiWindow window;
	window.present();
	if(argc > 1)
	{
		window.OpenPath(argv[1]);
	}
	app->run(window);
}

