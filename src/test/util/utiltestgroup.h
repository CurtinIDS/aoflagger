#ifndef AOFLAGGER_UTILTESTGROUP_H
#define AOFLAGGER_UTILTESTGROUP_H

#include "../testingtools/testgroup.h"

#include "numberparsertest.h"

class UtilTestGroup : public TestGroup {
	public:
		UtilTestGroup() : TestGroup("Common utilities") { }
		
		virtual void Initialize()
		{
			Add(new NumberParserTest());
		}
};

#endif
