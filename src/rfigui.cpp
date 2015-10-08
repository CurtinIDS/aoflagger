#include "version.h"

#include "gui/rfiguiwindow.h"

#include "gui/controllers/rfiguicontroller.h"

#include "util/aologger.h"

#include "strategy/imagesets/msimageset.h"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <gtkmm/application.h>

#include <glibmm/error.h>
#include <glibmm/wrap.h>

#include <string>
#include <vector>

#include <libgen.h>

struct SavedBaseline
{
	size_t a1Index, a2Index, bandIndex, sequenceIndex;
	std::string filename;
	bool operator<(const SavedBaseline& rhs) const
	{
		return boost::tie(a1Index, a2Index, bandIndex, sequenceIndex, filename) <
			boost::tie(rhs.a1Index, rhs.a2Index, rhs.bandIndex, rhs.sequenceIndex, rhs.filename);
	}
};

static void run(int argc, char *argv[])
{
	int argi = 1;
	
	std::vector<std::string> filenames;
	std::set<SavedBaseline> savedBaselines;
	bool interactive = true;
	std::string dataColumnName = "DATA";
	
	while(argi < argc)
	{
		if(argv[argi][0] == '-')
		{
			std::string p;
			if(argv[argi][1] == '-')
				p = &argv[argi][2];
			else
				p = &argv[argi][1];
			if(p == "h" || p == "help" || p == "?")
			{
				AOLogger::Info
					<< "The RFIGui is a program to analyze the time-frequency information in radio astronomical observations.\n"
					<< "It is written by André Offringa (offringa@gmail.com) and published under the GPL 3.\n"
					<< "This program is part of AOFlagger " << AOFLAGGER_VERSION_STR << " (" << AOFLAGGER_VERSION_DATE_STR << ")\n\n"
					<< "Syntax:\n"
					<< "  rfigui [-option1 [-option2 ...]] [measurement set]\n\n"
					<< "The main window will be opened when no parameters are specified.\n"
					<< "Possible options:\n"
					<< " -help\n"
					<< "    Display this help message and exit.\n"
					<< " -version\n"
					<< "    Display AOFlagger version and exit.\n"
					<< " -save-baseline <filename> <antenna1> <antenna2> <band> <sequence index>\n"
					<< "    Save the selected baseline to the given filename. The parameter can be specified\n"
					<< "    multiple times to save multiple baselines in one run. When this parameter is specified,\n"
					<< "    the main window will not open and the program will exit after saving the requested baselines.\n"
					<< " -data-column <columnname>\n"
					<< "    Open the selected column name.\n";
				return;
			}
			else if(p == "version")
			{
				AOLogger::Info << "AOFlagger " << AOFLAGGER_VERSION_STR << " (" << AOFLAGGER_VERSION_DATE_STR << ")\n";
				return;
			}
			else if(p == "save-baseline")
			{
				SavedBaseline sb;
				sb.filename = argv[argi+1];
				sb.a1Index = atoi(argv[argi+2]);
				sb.a2Index = atoi(argv[argi+3]);
				sb.bandIndex = atoi(argv[argi+4]);
				sb.sequenceIndex = atoi(argv[argi+5]);
				savedBaselines.insert(sb);
				interactive = false;
				argi += 5;
			}
			else if(p == "data-column")
			{
				++argi;
				dataColumnName = argv[argi];
			}
			else {
				AOLogger::Error << "Unknown parameter " << argv[argi] << " specified on command line.\n";
				return;
			}
		}
		else {
			filenames.push_back(argv[argi]);
		}
		++argi;
	}
	
	// We have to 'lie' about argc to create(..), because of a bug in older gtkmms.
	int altArgc = 1;
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(altArgc, argv, "", Gio::APPLICATION_HANDLES_OPEN);
	RFIGuiWindow window;
	if(interactive)
		window.present();
	
	try {
		
		if(!filenames.empty())
		{
			if(filenames.size() > 1)
				throw std::runtime_error("Error: multiple input paths specified; RFIGui can only handle one path.\n");
			if(interactive)
				window.OpenPath(filenames[0]);
			else
				window.Controller().Open(filenames[0], DirectReadMode, true, dataColumnName, false, 4, false, true);
		}
		
		if(!savedBaselines.empty())
		{
			rfiStrategy::MSImageSet* imageSet =
				dynamic_cast<rfiStrategy::MSImageSet*>(&window.GetImageSet());
			if(imageSet == 0)
				throw std::runtime_error("Option -save-baseline can only be used for measurement sets.\n");
			window.GetTimeFrequencyWidget().SetShowXAxisDescription(true);
			window.GetTimeFrequencyWidget().SetShowYAxisDescription(true);
			window.GetTimeFrequencyWidget().SetShowZAxisDescription(true);
			for(std::set<SavedBaseline>::const_iterator i=savedBaselines.begin(); i!=savedBaselines.end(); ++i)
			{
				window.SetImageSetIndex(imageSet->Index(i->a1Index, i->a2Index, i->bandIndex, i->sequenceIndex));
				window.GetTimeFrequencyWidget().SaveByExtension(i->filename, 800, 480);
			}
		}
		
		if(interactive)
			app->run(window);
	} catch(const std::exception& e)
	{
		AOLogger::Error <<
			"\n"
			"==========\n"
			"An unhandled exception occured while executing RFIGui. The error is:\n" <<
			e.what() << '\n';
	}
}

int main(int argc, char *argv[])
{
	AOLogger::Init(basename(argv[0]), false, true);
	
	run(argc, argv);
	
	Glib::Error::register_cleanup();
	Glib::wrap_register_cleanup();
	
	return 0;
}
