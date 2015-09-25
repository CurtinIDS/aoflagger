#ifndef AOFLAGGER_MSIOTESTGROUP_H
#define AOFLAGGER_MSIOTESTGROUP_H

#include "../testingtools/testgroup.h"

class MSIOTestGroup : public TestGroup {
	public:
		MSIOTestGroup() : TestGroup("Measurement set input/output") { }
		
		virtual void Initialize()
		{
		}
};

#endif
