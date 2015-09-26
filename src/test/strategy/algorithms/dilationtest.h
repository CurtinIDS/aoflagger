#ifndef AOFLAGGER_DILATIONTEST_H
#define AOFLAGGER_DILATIONTEST_H

#include "../../testingtools/asserter.h"
#include "../../testingtools/unittest.h"

#include "../../../structures/mask2d.h"

#include "../../../strategy/algorithms/statisticalflagger.h"

class DilationTest : public UnitTest {
	public:
		DilationTest() : UnitTest("Dilation algorithm")
		{
			AddTest(TestHorizontalDilation(), "Horizontal dilation");
			AddTest(TestVerticalDilation(), "Vertical dilation");
		}
		
	private:
		struct TestSingleDilation : public Asserter
		{
			template<bool Flip, typename DilateFunction>
			void testDilation(DilateFunction dilate);
		};
		struct TestHorizontalDilation : public DilationTest::TestSingleDilation
		{
			void operator()();
		};
		struct TestVerticalDilation : public DilationTest::TestSingleDilation
		{
			void operator()();
		};
		
		static std::string maskToString(Mask2DCPtr mask, bool flip)
		{
			std::stringstream s;
			if(flip)
			{
				for(unsigned x=0;x<mask->Width();++x)
				{
					for(unsigned y=0;y<mask->Height();++y)
						s << (mask->Value(x, y) ? 'x' : ' ');
					s << '\n';
				}
			} else
			{
				for(unsigned y=0;y<mask->Height();++y)
				{
					for(unsigned x=0;x<mask->Width();++x)
						s << (mask->Value(x, y) ? 'x' : ' ');
					s << '\n';
				}
			}
			return s.str();
		}
		
		static void setMask(Mask2DPtr mask, bool flip, const std::string &str)
		{
			std::string::const_iterator i = str.begin();
			if(flip)
			{
				for(unsigned x=0;x<mask->Width();++x)
				{
					for(unsigned y=0;y<mask->Height();++y)
					{
						mask->SetValue(x, y, (*i) == 'x');
						++i;
					}
					++i; // newline
				}
			} else
			{
				for(unsigned y=0;y<mask->Height();++y)
				{
					for(unsigned x=0;x<mask->Width();++x)
					{
						mask->SetValue(x, y, (*i) == 'x');
						++i;
					}
					++i; // newline
				}
			}
		}
};

inline void DilationTest::TestHorizontalDilation::operator()()
{
	TestSingleDilation::testDilation<false>(StatisticalFlagger::DilateFlagsHorizontally);
}

inline void DilationTest::TestVerticalDilation::operator()()
{
	TestSingleDilation::testDilation<true>(StatisticalFlagger::DilateFlagsVertically);
}

template<bool Flip, typename DilateFunction>
inline void DilationTest::TestSingleDilation::testDilation(DilateFunction dilate)
{
	std::string expected;
	Mask2DPtr mask = Mask2D::CreateSetMaskPtr<false>(5, 5);
	dilate(mask, 0);
	expected =
		"     \n"
		"     \n"
		"     \n"
		"     \n"
		"     \n";
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=0");
	dilate(mask, 1);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=1");
	dilate(mask, 2);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=2");
	dilate(mask, 3);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=3");
	dilate(mask, 4);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=4");
	dilate(mask, 5);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=5");
	dilate(mask, 6);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with size=6");
	
	expected =
		"     \n"
		"     \n"
		"  x  \n"
		"     \n"
		"     \n";
	setMask(mask, Flip, expected);
	dilate(mask, 0);
	AssertEquals(maskToString(mask, Flip), expected, "Empty dilation with single flag");

	dilate(mask, 1);
	expected =
		"     \n"
		"     \n"
		" xxx \n"
		"     \n"
		"     \n";
	AssertEquals(maskToString(mask, Flip), expected, "Dilation with single flag, size=1");

	dilate(mask, 1);
	expected =
		"     \n"
		"     \n"
		"xxxxx\n"
		"     \n"
		"     \n";
	AssertEquals(maskToString(mask, Flip), expected, "Dilation with three flags, size=1");
	dilate(mask, 1);
	AssertEquals(maskToString(mask, Flip), expected, "Dilation with five flags at the borders, size=1");
	dilate(mask, 100);
	AssertEquals(maskToString(mask, Flip), expected, "Dilation with five flags at the borders, size=100");

	setMask(mask, Flip,
		" x   \n"
		"   x \n"
		"x    \n"
		"    x\n"
		" x x \n");
	dilate(mask, 2);
	expected =
		"xxxx \n"
		" xxxx\n"
		"xxx  \n"
		"  xxx\n"
		"xxxxx\n";
	AssertEquals(maskToString(mask, Flip), expected, "Somewhat random input");
	
	setMask(mask, Flip,
		"x    \n"
		" x   \n"
		"  x  \n"
		"   x \n"
		"    x\n");
	dilate(mask, 6);
	expected =
		"xxxxx\n"
		"xxxxx\n"
		"xxxxx\n"
		"xxxxx\n"
		"xxxxx\n";
	AssertEquals(maskToString(mask, Flip), expected, "Fully flagged");
	
	if(Flip)
		mask = Mask2D::CreateSetMaskPtr<false>(4, 6);
	else
		mask = Mask2D::CreateSetMaskPtr<false>(6, 4);
	dilate(mask, 0);
	expected =
		"      \n"
		"      \n"
		"      \n"
		"      \n";
	AssertEquals(maskToString(mask, Flip), expected, "(6,4) empty");
	
	setMask(mask, Flip,
		"x     \n"
		"  x   \n"
		"    x \n"
		"     x\n");
	dilate(mask, 4);
	expected =
		"xxxxx \n"
		"xxxxxx\n"
		"xxxxxx\n"
		" xxxxx\n";
	AssertEquals(maskToString(mask, Flip), expected, "(6,4) large size");

	setMask(mask, Flip,
		"x     \n"
		"      \n"
		"    x \n"
		"     x\n");
	dilate(mask, 7);
	expected =
		"xxxxxx\n"
		"      \n"
		"xxxxxx\n"
		"xxxxxx\n";
	AssertEquals(maskToString(mask, Flip), expected, "(6,4) full size");
}

#endif
