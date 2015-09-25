#ifndef AOFLAGGER_ASSERTER_H
#define AOFLAGGER_ASSERTER_H

#include "equalsasserter.h"
#include "lessthanasserter.h"

class Asserter : public EqualsAsserter, public LessThanAsserter {
	public:
		Asserter()
		{
		}
		
		~Asserter()
		{
		}
};

#endif
