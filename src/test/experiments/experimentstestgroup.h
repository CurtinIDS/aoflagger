#ifndef AOFLAGGER_EXPERIMENTSTESTGROUP_H
#define AOFLAGGER_EXPERIMENTSTESTGROUP_H

#include "../testingtools/testgroup.h"

#include "defaultstrategyspeedtest.h"
//#include "filterresultstest.h"
#include "highpassfilterexperiment.h"
//#include "scaleinvariantdilationexperiment.h"
//#include "rankoperatorrocexperiment.h"

class ExperimentsTestGroup : public TestGroup {
	public:
		ExperimentsTestGroup() : TestGroup("Experiments") { }
		
		virtual void Initialize()
		{
			Add(new HighPassFilterExperiment());
			//Add(new RankOperatorROCExperiment());
			Add(new DefaultStrategySpeedTest());
			//Add(new FilterResultsTest());
			//Add(new ScaleInvariantDilationExperiment());
		}
};

#endif
