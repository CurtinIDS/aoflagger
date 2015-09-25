#include <libgen.h>

#include "gui/application.h"

#include "util/aologger.h"

#include <glibmm/error.h>
#include <glibmm/wrap.h>

int main(int argc, char *argv[])
{
	
	AOLogger::Init(basename(argv[0]), false, true);
	{
		Application application;
		application.Run(argc, argv);
	}
	
	Glib::Error::register_cleanup();
	Glib::wrap_register_cleanup();
	
	return 0;
}
