/***************************************************************************
 *   Copyright (C) 2008 by A.R. Offringa   *
 *   offringa@astro.rug.nl   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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

