#ifndef AOFLAGGER_SUMTHRESHOLDTEST_H
#define AOFLAGGER_SUMTHRESHOLDTEST_H

#include "../../../msio/image2d.h"
#include "../../../msio/mask2d.h"
#include "../../../msio/timefrequencydata.h"

#include "../../../strategy/algorithms/mitigationtester.h"
#include "../../../strategy/algorithms/thresholdconfig.h"
#include "../../../strategy/algorithms/thresholdmitigater.h"

#include "../../testingtools/asserter.h"
#include "../../testingtools/maskasserter.h"
#include "../../testingtools/unittest.h"

class SumThresholdTest : public UnitTest {
	public:
		SumThresholdTest() : UnitTest("Sumthreshold")
		{
			AddTest(VerticalSumThresholdSSE(), "SumThreshold optimized SSE version (vertical)");
			AddTest(HorizontalSumThresholdSSE(), "SumThreshold optimized SSE version (horizontal)");
			AddTest(Stability(), "SumThreshold stability");
		}
		
	private:
		struct VerticalSumThresholdSSE : public Asserter
		{
			void operator()();
		};
		struct HorizontalSumThresholdSSE : public Asserter
		{
			void operator()();
		};
		struct Stability : public Asserter
		{
			void operator()();
		};
};

void SumThresholdTest::VerticalSumThresholdSSE::operator()()
{
	const unsigned
		width = 2048,
		height = 256;
	Mask2DPtr
		mask1 = Mask2D::CreateUnsetMaskPtr(width, height),
		mask2 = Mask2D::CreateUnsetMaskPtr(width, height);
	Image2DPtr
		real = MitigationTester::CreateTestSet(26, mask1, width, height),
		imag = MitigationTester::CreateTestSet(26, mask2, width, height);
	TimeFrequencyData data(XXPolarisation, real, imag);
	Image2DCPtr image = data.GetSingleImage();
	
	ThresholdConfig config;
	config.InitializeLengthsDefault(9);
	num_t mode = image->GetMode();
	config.InitializeThresholdsFromFirstThreshold(6.0 * mode, ThresholdConfig::Rayleigh);
	for(unsigned i=0;i<9;++i)
	{
		mask1->SetAll<false>();
		mask2->SetAll<false>();
		
		const unsigned length = config.GetHorizontalLength(i);
		const double threshold = config.GetHorizontalThreshold(i);
		
		ThresholdMitigater::VerticalSumThresholdLargeReference(image, mask1, length, threshold);
		ThresholdMitigater::VerticalSumThresholdLargeSSE(image, mask2, length, threshold);
		
		if(length != 32) {
		  std::stringstream s;
		  s << "Equal SSE and reference masks produced by SumThreshold length " << length;
		  AssertTrue(mask1->Equals(mask2), s.str());
		}
	}
}

void SumThresholdTest::HorizontalSumThresholdSSE::operator()()
{
	const unsigned
		width = 2048,
		height = 256;
	Mask2DPtr
		mask1 = Mask2D::CreateUnsetMaskPtr(width, height),
		mask2 = Mask2D::CreateUnsetMaskPtr(width, height);
	Image2DPtr
		real = MitigationTester::CreateTestSet(26, mask1, width, height),
		imag = MitigationTester::CreateTestSet(26, mask2, width, height);
		
	mask1->SwapXY();
	mask2->SwapXY();
	real->SwapXY();
	imag->SwapXY();
		
	TimeFrequencyData data(XXPolarisation, real, imag);
	Image2DCPtr image = data.GetSingleImage();

	ThresholdConfig config;
	config.InitializeLengthsDefault(9);
	num_t mode = image->GetMode();
	config.InitializeThresholdsFromFirstThreshold(6.0 * mode, ThresholdConfig::Rayleigh);
	for(unsigned i=0;i<9;++i)
	{
		mask1->SetAll<false>();
		mask2->SetAll<false>();
		
		const unsigned length = config.GetHorizontalLength(i);
		const double threshold = config.GetHorizontalThreshold(i);
		
		ThresholdMitigater::HorizontalSumThresholdLargeReference(image, mask1, length, threshold);
		ThresholdMitigater::HorizontalSumThresholdLargeSSE(image, mask2, length, threshold);
		
		std::stringstream s;
		s << "Equal SSE and reference masks produced by SumThreshold length " << length << ", threshold " << threshold;
		MaskAsserter::AssertEqualMasks(mask2, mask1, s.str());
	}
}

void SumThresholdTest::Stability::operator()()
{
	Mask2DPtr
		maskA = Mask2D::CreateSetMaskPtr<false>(1, 1),
		maskB = Mask2D::CreateSetMaskPtr<false>(2, 2),
		maskC = Mask2D::CreateSetMaskPtr<false>(3, 3),
		maskD = Mask2D::CreateSetMaskPtr<false>(4, 4);
	Image2DPtr
		realA = Image2D::CreateZeroImagePtr(1, 1),
		realB = Image2D::CreateZeroImagePtr(2, 2),
		realC = Image2D::CreateZeroImagePtr(3, 3),
		realD = Image2D::CreateZeroImagePtr(4, 4);
		
	ThresholdConfig config;
	config.InitializeLengthsDefault(9);
	config.InitializeThresholdsFromFirstThreshold(6.0, ThresholdConfig::Rayleigh);
	for(unsigned i=0;i<9;++i)
	{
		const unsigned length = config.GetHorizontalLength(i);
		ThresholdMitigater::HorizontalSumThresholdLargeSSE(realA, maskA, length, 1.0);
		ThresholdMitigater::VerticalSumThresholdLargeSSE(realA, maskA, length, 1.0);
		ThresholdMitigater::HorizontalSumThresholdLargeSSE(realA, maskB, length, 1.0);
		ThresholdMitigater::VerticalSumThresholdLargeSSE(realA, maskB, length, 1.0);
		ThresholdMitigater::HorizontalSumThresholdLargeSSE(realA, maskC, length, 1.0);
		ThresholdMitigater::VerticalSumThresholdLargeSSE(realA, maskC, length, 1.0);
		ThresholdMitigater::HorizontalSumThresholdLargeSSE(realA, maskD, length, 1.0);
		ThresholdMitigater::VerticalSumThresholdLargeSSE(realA, maskD, length, 1.0);
	}
}

#endif
