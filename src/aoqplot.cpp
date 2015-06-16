/***************************************************************************
 *   Copyright (C) 2011 by A.R. Offringa                                   *
 *   offringa@astro.rug.nl                                                 *
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

#include "gui/quality/aoqplotwindow.h"

#include <gtkmm/main.h>
#include <gtkmm/filechooserdialog.h>

#include "version.h"

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("", Gio::APPLICATION_HANDLES_OPEN);
	AOQPlotWindow window;
	bool openGUI = true;
	int argi = 1;
	std::vector<AOQPlotWindow::PlotSavingData> savedPlots;
	while(argi < argc && argv[argi][0]=='-')
	{
		std::string p;
		if(argv[argi][1] == '-')
			p = &argv[argi][2];
		else
			p = &argv[argi][1];
		if(p=="help" || p=="h")
		{
			std::cout << "Syntax: aoqplot [<options>] [<observation>]\n\n"
				"<observation> can be a measurement set for opening a single observation.\n"
				"To get statistics for a (remote) observation consisting of multiple measurement\n"
				"sets, specify a measurement set specifier instead (generally a .ref, .vds\n"
				".gvds or .gds file).\n\n"
				"aoqplot is part of the AOFlagger software package, written\n"
				"by André Offringa (offringa@gmail.com).\n";
			return 0;
		}
		else if(p=="save")
		{
			AOQPlotWindow::PlotSavingData newPlot;
			newPlot.filenamePrefix = argv[argi+1];
			newPlot.statisticKind = QualityTablesFormatter::NameToKind(argv[argi+2]);
			argi += 2;
			openGUI = false;
			savedPlots.push_back(newPlot);
		}
		else if(p == "version")
		{
			std::cout << "AOFlagger " << AOFLAGGER_VERSION_STR << " (" << AOFLAGGER_VERSION_DATE_STR << ")\n";
			return 0;
		}
		else {
			std::cout << "Bad parameter specified: " << argv[argi] << '\n';
			return 1;
		}
		++argi;
	}
	if(openGUI)
		window.show();
	
	if(argc>argi)
	{
		std::vector<std::string> files;
		for(int i=argi; i!=argc; ++i)
			files.push_back(argv[i]);
		if(openGUI)
			window.Open(files);
		else
			window.OpenWithoutGUI(files);
	} else {
		Gtk::FileChooserDialog fileDialog(window, "Open observation set");
		
		fileDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
		fileDialog.add_button("_Open", Gtk::RESPONSE_OK);
		
		Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create();
		filter->set_name("Observation sets (*.{vds,gds,ref,MS})");
		filter->add_pattern("*.vds");
		filter->add_pattern("*.gds");
		filter->add_pattern("*.gvds");
		filter->add_pattern("*.ref");
		filter->add_pattern("*.MS");
		fileDialog.add_filter(filter);
		
		if(fileDialog.run() == Gtk::RESPONSE_OK)
		{
			window.Open(fileDialog.get_filename());
		}
		else return 0;
	}
	if(openGUI)
		app->run(window);
	return 0;
}
