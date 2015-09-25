#ifndef AOFLAGGER_NUMBERPARSERTEST_H
#define AOFLAGGER_NUMBERPARSERTEST_H

#include "../testingtools/asserter.h"
#include "../testingtools/unittest.h"

#include "../../util/numberparser.h"

class NumberParserTest : public UnitTest {
	public:
		NumberParserTest() : UnitTest("Number parser")
		{
			AddTest(TestToDouble(), "Parsing double");
		}
		
	private:
		struct TestToDouble : public Asserter
		{
			void operator()();
		};
};

inline void NumberParserTest::TestToDouble::operator()()
{
	AssertEquals<double>(NumberParser::ToDouble("1"), 1.0);
	AssertEquals<double>(NumberParser::ToDouble("1."), 1.0);
	AssertEquals<double>(NumberParser::ToDouble("1.000000"), 1.0);
	AssertEquals<double>(NumberParser::ToDouble("-1"), -1.0);
	AssertEquals<double>(NumberParser::ToDouble("-1.00000"), -1.0);
	
	AssertAlmostEqual(NumberParser::ToDouble("3.14159265"), 3.14159265);
	AssertAlmostEqual(NumberParser::ToDouble("0.00002"), 0.00002);
	AssertEquals<double>(NumberParser::ToDouble("234567"), 234567.0);
	AssertAlmostEqual(NumberParser::ToDouble("234.567"), 234.567);
	
	AssertEquals<double>(NumberParser::ToDouble("0"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("0.0"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("-0.0"), 0.0);
	
	AssertEquals<double>(NumberParser::ToDouble("0.0e5"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("0.0e100"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("0.0e-100"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("0.0E5"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("0.0E100"), 0.0);
	AssertEquals<double>(NumberParser::ToDouble("0.0E-100"), 0.0);
	
	AssertAlmostEqual(NumberParser::ToDouble("1.0e5"), 1.0e5);
	AssertAlmostEqual(NumberParser::ToDouble("1.0e-5"), 1.0e-5);
	AssertAlmostEqual(NumberParser::ToDouble("0.3e0"), 0.3);
	AssertAlmostEqual(NumberParser::ToDouble("642.135e8"), 642.135e8);
}

#endif
