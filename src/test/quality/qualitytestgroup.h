#ifndef AOFLAGGER_QUALITYTESTGROUP_H
#define AOFLAGGER_QUALITYTESTGROUP_H

#include "../testingtools/testgroup.h"

#include "qualitytablesformattertest.h"
#include "statisticscollectiontest.h"
#include "statisticsderivatortest.h"

class QualityTestGroup : public TestGroup {
	public:
		QualityTestGroup() : TestGroup("Quality statistic tools") { }
		
		virtual void Initialize()
		{
			Add(new QualityTablesFormatterTest());
			Add(new StatisticsCollectionTest());
			Add(new StatisticsDerivatorTest());
		}
};

#endif
