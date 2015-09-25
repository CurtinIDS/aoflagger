#ifndef AOFLAGGER_ALGORITHMSTESTGROUP_H
#define AOFLAGGER_ALGORITHMSTESTGROUP_H

#include "../../testingtools/testgroup.h"

#include "convolutionstest.h"
#include "dilationtest.h"
#include "eigenvaluetest.h"
#include "highpassfiltertest.h"
#include "noisestatisticstest.h"
#include "siroperatortest.h"
#include "statisticalflaggertest.h"
#include "sumthresholdtest.h"
#include "thresholdtoolstest.h"

class AlgorithmsTestGroup : public TestGroup {
	public:
		AlgorithmsTestGroup() : TestGroup("Algorithms") { }
		
		virtual void Initialize()
		{
			Add(new ConvolutionsTest());
			Add(new DilationTest());
			Add(new EigenvalueTest());
			Add(new HighPassFilterTest());
			Add(new NoiseStatisticsTest());
			Add(new SIROperatorTest());
			Add(new StatisticalFlaggerTest());
			Add(new SumThresholdTest());
			Add(new ThresholdToolsTest());
		}
};

#endif
