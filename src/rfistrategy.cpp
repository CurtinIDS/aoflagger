#include <iostream>
#include <libgen.h>

#include "strategy/actions/strategy.h"

#include "strategy/control/strategyreader.h"
#include "strategy/control/strategywriter.h"
#include "strategy/control/defaultstrategy.h"

#include "util/aologger.h"
#include "util/parameter.h"

using namespace rfiStrategy;
using namespace std;

int main(int argc, char *argv[])
{
	AOLogger::Init(basename(argv[0]), false);

	cout << 
			"RFI strategy file writer\n"
			"This program will write an RFI strategy to a file, to run it with the\n"
			"rficonsole or the rfigui.\n\n"
			"Author: André Offringa (offringa@astro.rug.nl)\n"
			<< endl;

	Parameter<enum BaselineSelection> baselineSelection;
	Parameter<std::string> dataColumn;
	Parameter<bool> frequencyBasedFlagging;
	Parameter<bool> flagStokes;
	Parameter<size_t> threadCount;
	Parameter<pair<double, double> > kernelSize;
	Parameter<enum PolarisationType> polarisation;
	Parameter<num_t> sensitivity;
	Parameter<pair<size_t, size_t> > windowSize;

	size_t parameterIndex = 1;
	while(parameterIndex < (size_t) argc && argv[parameterIndex][0]=='-')
	{
		string flag(argv[parameterIndex]+1);

		{
			cerr << "Incorrect usage; parameter \"" << argv[parameterIndex] << "\" not understood.\nType rfistrategy without parameters for a list of commands." << endl;
			return 1;
		}
		++parameterIndex;
	}
	if((int) parameterIndex > argc-2)
	{
		cerr << "Usage: " << argv[0] << " <profile> <filename>\n\n"
			"Profiles:\n"
			"default	- currently only profile\n"
			"<filename> is the filename to which the strategy is written. This\n"
			"file should have the extension \".rfis\".\n\n"
			"\n"
			"Possible options:\n";
		return 1;
	}

	string profile(argv[parameterIndex]), filename(argv[parameterIndex+1]);

	rfiStrategy::Strategy *strategy;
	if(profile == "default")
	{
		strategy = new Strategy();
		DefaultStrategy::LoadFullStrategy(*strategy, DefaultStrategy::GENERIC_TELESCOPE, DefaultStrategy::FLAG_NONE);
	}
	else {
		cerr << "Unknown profile: " << profile << endl;
		return 1;
	}

	rfiStrategy::StrategyWriter writer;
	cout << "Writing strategy..." << endl;
	writer.WriteToFile(*strategy, filename);
	delete strategy;
}
