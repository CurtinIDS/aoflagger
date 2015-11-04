#include <iostream>
#include <string>

#include <libgen.h>

#include "strategy/actions/foreachmsaction.h"
#include "strategy/actions/strategy.h"

#include "strategy/algorithms/baselineselector.h"
#include "strategy/algorithms/polarizationstatistics.h"

#include "strategy/plots/antennaflagcountplot.h"
#include "strategy/plots/frequencyflagcountplot.h"
#include "strategy/plots/timeflagcountplot.h"

#include "strategy/control/artifactset.h"
#include "strategy/control/strategyreader.h"
#include "strategy/control/defaultstrategy.h"

#include "util/aologger.h"
#include "util/parameter.h"
#include "util/progresslistener.h"
#include "util/stopwatch.h"
#include "util/numberlist.h"

#include "version.h"

#include <boost/date_time/posix_time/posix_time.hpp>

class ConsoleProgressHandler : public ProgressListener {
	private:
		boost::mutex _mutex;
		
	public:
		
		virtual void OnStartTask(const rfiStrategy::Action &action, size_t taskNo, size_t taskCount, const std::string &description, size_t weight)
		{
			boost::mutex::scoped_lock lock(_mutex);
			ProgressListener::OnStartTask(action, taskNo, taskCount, description, weight);
			
			double totalProgress = TotalProgress();
			
			AOLogger::Progress << round(totalProgress*1000.0)/10.0 << "% : ";
			
			for(size_t i=1;i<Depth();++i)
				AOLogger::Progress << "+-";
			
			AOLogger::Progress << description << "...\n";
		}
		
		virtual void OnEndTask(const rfiStrategy::Action &action)
		{
			boost::mutex::scoped_lock lock(_mutex);
			
			ProgressListener::OnEndTask(action);
		}

		virtual void OnProgress(const rfiStrategy::Action &action, size_t i, size_t j)
		{
			ProgressListener::OnProgress(action, i, j);
		}

		virtual void OnException(const rfiStrategy::Action &, std::exception &thrownException) 
		{
			AOLogger::Error <<
				"An exception occured during execution of the strategy!\n"
				"Your set might not be fully flagged. Exception was:\n"
				<< thrownException.what() << '\n';
		}
};

#define RETURN_SUCCESS                0
#define RETURN_CMDLINE_ERROR         10
#define RETURN_STRATEGY_PARSE_ERROR  20
#define RETURN_UNHANDLED_EXCEPTION   30

void checkRelease()
{
#ifndef NDEBUG
		AOLogger::Warn
			<< "This version of the AOFlagger has been compiled as DEBUG version! (NDEBUG was not defined)\n"
			<< "For better performance, recompile it as a RELEASE.\n\n";
#endif
}
void generalInfo()
{
	AOLogger::Info << 
		"AOFlagger " << AOFLAGGER_VERSION_STR << " (" << AOFLAGGER_VERSION_DATE_STR <<
		") command line application\n"
		"This program will execute an RFI strategy as can be created with the RFI gui\n"
		"and executes it on one or several observations.\n\n"
		"Author: André Offringa (offringa@gmail.com)\n\n";
}

int main(int argc, char **argv)
{
	if(argc == 1)
	{
		AOLogger::Init(basename(argv[0]));
		generalInfo();
		AOLogger::Error << "Usage: " << argv[0] << " [options] <obs1> [<obs2> [..]]\n"
		"  -v will produce verbose output\n"
		"  -j overrides the number of threads specified in the strategy\n"
		"     (default: one thread for each CPU core)\n"
		"  -strategy specifies a possible customized strategy\n"
		"  -direct-read will perform the slowest IO but will always work.\n"
		"  -indirect-read will reorder the measurement set before starting, which is normally\n"
		"     faster but requires free disk space to reorder the data to.\n"
		"  -memory-read will read the entire measurement set in memory. This is the fastest, but\n"
		"     requires much memory.\n"
		"  -auto-read-mode will select either memory or direct mode based on available memory (default).\n"
		"  -skip-flagged will skip an ms if it has already been processed by AOFlagger according\n"
		"     to its HISTORY table.\n"
		"  -uvw reads uvw values (some exotic strategies require these)\n"
		"  -column <name> specify column to flag\n"
		"  -bands <list> comma separated list of (zero-indexed) band ids to process\n"
		"  -fields <list> comma separated list of (zero-indexed) field ids to process\n"
		"\n"
		"This tool supports at least the Casa measurement set, the SDFITS and Filterbank formats. See\n"
		"the documentation for support of other file types.\n";
		
		checkRelease();
		
		return RETURN_CMDLINE_ERROR;
	}
	
#ifdef HAS_LOFARSTMAN
	register_lofarstman();
#endif // HAS_LOFARSTMAN
	
	Parameter<size_t> threadCount;
	Parameter<BaselineIOMode> readMode;
	Parameter<bool> readUVW;
	Parameter<std::string> strategyFile;
	Parameter<bool> logVerbose;
	Parameter<bool> skipFlagged;
	Parameter<std::string> dataColumn;
	std::set<size_t> bands, fields;

	size_t parameterIndex = 1;
	while(parameterIndex < (size_t) argc && argv[parameterIndex][0]=='-')
	{
		std::string flag(argv[parameterIndex]+1);
		
		// If "--" was used, strip another dash
		if(!flag.empty() && flag[0] == '-')
			flag = flag.substr(1);
		
		if(flag=="j" && parameterIndex < (size_t) (argc-1))
		{
			++parameterIndex;
			threadCount = atoi(argv[parameterIndex]);
		}
		else if(flag=="v")
		{
			logVerbose = true;
		}
		else if(flag == "version")
		{
			AOLogger::Init(basename(argv[0]));
			AOLogger::Info << "AOFlagger " << AOFLAGGER_VERSION_STR << " (" << AOFLAGGER_VERSION_DATE_STR << ")\n";
			return 0;
		}
		else if(flag=="direct-read")
		{
			readMode = DirectReadMode;
		}
		else if(flag=="indirect-read")
		{
			readMode = IndirectReadMode;
		}
		else if(flag=="memory-read")
		{
			readMode = MemoryReadMode;
		}
		else if(flag=="auto-read-mode")
		{
			readMode = AutoReadMode;
		}
		else if(flag=="strategy")
		{
			parameterIndex++;
			strategyFile = argv[parameterIndex];
		}
		else if(flag=="skip-flagged")
		{
			skipFlagged = true;
		}
		else if(flag=="uvw")
		{
			readUVW = true;
		}
		else if(flag == "column")
		{
			parameterIndex++;
			std::string columnStr(argv[parameterIndex]);
			dataColumn = columnStr; 
		}
		else if(flag == "bands")
		{
			++parameterIndex;
			NumberList::ParseIntList(argv[parameterIndex], bands);
		}
		else if(flag == "fields")
		{
			++parameterIndex;
			NumberList::ParseIntList(argv[parameterIndex], fields);
		}
		else
		{
			AOLogger::Init(basename(argv[0]));
			AOLogger::Error << "Incorrect usage; parameter \"" << argv[parameterIndex] << "\" not understood.\n";
			return 1;
		}
		++parameterIndex;
	}

	try {
		AOLogger::Init(basename(argv[0]), false, logVerbose.Value(false));
		generalInfo();
			
		checkRelease();

		if(!threadCount.IsSet())
			threadCount = sysconf(_SC_NPROCESSORS_ONLN);
		AOLogger::Debug << "Number of threads: " << threadCount.Value() << "\n";

		Stopwatch watch(true);

		boost::mutex ioMutex;
		
		rfiStrategy::ForEachMSAction *fomAction = new rfiStrategy::ForEachMSAction();
		if(readMode.IsSet())
			fomAction->SetIOMode(readMode);
		if(readUVW.IsSet())
			fomAction->SetReadUVW(readUVW);
		if(dataColumn.IsSet())
			fomAction->SetDataColumnName(dataColumn);
		if(!bands.empty())
			fomAction->Bands() = bands;
		if(!fields.empty())
			fomAction->Fields() = fields;
		std::stringstream commandLineStr;
		commandLineStr << argv[0];
		for(int i=1;i<argc;++i)
		{
			commandLineStr << " \"" << argv[i] << '\"';
		}
		fomAction->SetCommandLineForHistory(commandLineStr.str());
		if(skipFlagged.IsSet())
			fomAction->SetSkipIfAlreadyProcessed(skipFlagged);
		for(int i=parameterIndex;i<argc;++i)
		{
			AOLogger::Debug << "Adding '" << argv[i] << "'\n";
			fomAction->Filenames().push_back(argv[i]);
		}
		
		if(strategyFile.IsSet())
		{
			fomAction->SetLoadOptimizedStrategy(false);
			rfiStrategy::StrategyReader reader;
			rfiStrategy::Strategy *subStrategy;
			try {
				AOLogger::Debug << "Opening strategy file '" << strategyFile.Value() << "'\n";
				subStrategy = reader.CreateStrategyFromFile(strategyFile);
				AOLogger::Debug << "Strategy parsed succesfully.\n";
			} catch(std::exception &e)
			{
				AOLogger::Error <<
					"ERROR: Reading strategy file \"" << strategyFile.Value() << "\" failed! This\n"
					"might be caused by a change in the file format of the strategy file after you\n"
					"created the strategy file.\n"
					"Try recreating the file.\n"
					"\nThe thrown exception was:\n" << e.what() << "\n";
				return RETURN_STRATEGY_PARSE_ERROR;
			}
			if(!rfiStrategy::DefaultStrategy::StrategyContainsAction(*subStrategy, rfiStrategy::ForEachBaselineActionType) &&
				!rfiStrategy::DefaultStrategy::StrategyContainsAction(*subStrategy, rfiStrategy::WriteFlagsActionType))
			{
				rfiStrategy::DefaultStrategy::EncapsulateSingleStrategy(*fomAction, subStrategy, rfiStrategy::DefaultStrategy::GENERIC_TELESCOPE);
				AOLogger::Info << "Modified single-baseline strategy so it will execute strategy on all baselines and write flags.\n";
			}
			else {
				fomAction->Add(subStrategy);
			}
			if(threadCount.IsSet())
				rfiStrategy::Strategy::SetThreadCount(*fomAction, threadCount);
		}
		else {
			fomAction->SetLoadOptimizedStrategy(true);
			fomAction->Add(new rfiStrategy::Strategy()); // This helps the progress reader to determine progress
			if(threadCount.IsSet())
				fomAction->SetLoadStrategyThreadCount(threadCount);
		}
		
		rfiStrategy::Strategy overallStrategy;
		overallStrategy.Add(fomAction);

		rfiStrategy::ArtifactSet artifacts(&ioMutex);
		artifacts.SetAntennaFlagCountPlot(new AntennaFlagCountPlot());
		artifacts.SetFrequencyFlagCountPlot(new FrequencyFlagCountPlot());
		artifacts.SetTimeFlagCountPlot(new TimeFlagCountPlot());
		artifacts.SetPolarizationStatistics(new PolarizationStatistics());
		artifacts.SetBaselineSelectionInfo(new rfiStrategy::BaselineSelector());
		
		ConsoleProgressHandler progress;

		AOLogger::Info << "Starting strategy on " << to_simple_string(boost::posix_time::microsec_clock::local_time()) << '\n';
		
		overallStrategy.InitializeAll();
		overallStrategy.StartPerformThread(artifacts, progress);
		rfiStrategy::ArtifactSet *set = overallStrategy.JoinThread();
		overallStrategy.FinishAll();

		set->AntennaFlagCountPlot()->Report();
		set->FrequencyFlagCountPlot()->Report();
		set->PolarizationStatistics()->Report();

		delete set->AntennaFlagCountPlot();
		delete set->FrequencyFlagCountPlot();
		delete set->TimeFlagCountPlot();
		delete set->PolarizationStatistics();
		delete set->BaselineSelectionInfo();

		delete set;

		AOLogger::Debug << "Time: " << watch.ToString() << "\n";
		
		return RETURN_SUCCESS;
	} catch(std::exception &exception)
	{
		std::cerr
			<< "An unhandled exception occured: " << exception.what() << '\n'
			<< "If you think this is a bug, please contact offringa@gmail.com\n";
		return RETURN_UNHANDLED_EXCEPTION;
	}
}
