#include <iostream>

#include "strategy/algorithms/baselineselector.h"

#include "structures/measurementset.h"

#include "quality/defaultstatistics.h"
#include "quality/histogramcollection.h"
#include "quality/qualitytablesformatter.h"
#include "quality/statisticscollection.h"
#include "quality/statisticsderivator.h"

#include "remote/clusteredobservation.h"
#include "remote/processcommander.h"

#include "quality/histogramtablesformatter.h"

void checkStations(const std::string &filename)
{
	bool remote = aoRemote::ClusteredObservation::IsClusteredFilename(filename);
	StatisticsCollection statisticsCollection;
	HistogramCollection histogramCollection;
	std::vector<AntennaInfo> antennae;
	if(remote)
	{
		aoRemote::ClusteredObservation *observation = aoRemote::ClusteredObservation::Load(filename);
		aoRemote::ProcessCommander commander(*observation);
		commander.PushReadAntennaTablesTask();
		commander.PushReadQualityTablesTask(&statisticsCollection, &histogramCollection);
		commander.Run();
		antennae = commander.Antennas();
		delete observation;
	}
	else {
		MeasurementSet *ms = new MeasurementSet(filename);
		const unsigned polarizationCount = ms->PolarizationCount();
		
		statisticsCollection.SetPolarizationCount(polarizationCount);
		QualityTablesFormatter qualityData(filename);
		statisticsCollection.Load(qualityData);
		unsigned antennaCount = ms->AntennaCount();
		for(unsigned a=0;a<antennaCount;++a)
			antennae.push_back(ms->GetAntennaInfo(a));
		delete ms;
	}
	
	rfiStrategy::BaselineSelector selector;
	selector.SetUseLog(true);
	
	statisticsCollection.IntegrateBaselinesToOneChannel();
	const BaselineStatisticsMap &baselineMap = statisticsCollection.BaselineStatistics();
	const std::vector<std::pair<unsigned, unsigned> > list = baselineMap.BaselineList();
	for(std::vector<std::pair<unsigned, unsigned> >::const_iterator i=list.begin();i!=list.end();++i)
	{
		const unsigned a1 = i->first, a2 = i->second;
		
		DefaultStatistics statistic = baselineMap.GetStatistics(a1, a2);
		selector.Add(statistic, antennae[a1], antennae[a2]);
	}
	std::vector<rfiStrategy::BaselineSelector::SingleBaselineInfo> markedBaselines;
	std::set<unsigned> badStations;
	
	selector.Search(markedBaselines);
	selector.ImplyStations(markedBaselines, 0.3, badStations);
	
	std::cout << "List of " << badStations.size() << " bad stations:\n";
	for(std::set<unsigned>::const_iterator i=badStations.begin();i!=badStations.end();++i)
	{
		std::cout << antennae[*i].name << " (" << *i << ")\n";
	}
}

void printSyntax(std::ostream &stream, char *argv[])
{
	stream << "Syntax: badstations <filename>\n\n"
		"The binary 'badstations ' will print all stations that are outliers\n"
		"according to the RFI statistics. It takes into account that short baselines\n"
		"often see fewer RFI, by fitting a curve to the statistic as a function of\n"
		"baseline.\n";
}

int main(int argc, char *argv[])
{
#ifdef HAS_LOFARSTMAN
	register_lofarstman();
#endif // HAS_LOFARSTMAN

	if(argc != 2)
	{
		printSyntax(std::cerr, argv);
		return -1;
	} else {
		
		const std::string filename = argv[1];
		checkStations(filename);
		return 0;
	}
}
