#ifndef AOFLAGGER_MASK_ASSERTER_H
#define AOFLAGGER_MASK_ASSERTER_H

#include <string>
#include <stdexcept>
#include <sstream>

#include "../../structures/mask2d.h"

class MaskAsserter {
	public:
		MaskAsserter()
		{
		}
		
		~MaskAsserter()
		{
		}
		
		static void AssertEqualMasks(Mask2DPtr actual, Mask2DPtr expected, const std::string &str)
		{
			if(actual->Width() != expected->Width())
				throw std::runtime_error("Width of masks do not match");
			if(actual->Height() != expected->Height())
				throw std::runtime_error("Height of masks do not match");
			
			std::stringstream s;
			s << "AssertEqualMasks failed for test '" << str << "': ";
			
			size_t errCount = 0, falseMatches = 0, trueMatches = 0;
			for(size_t y=0;y<actual->Height();++y)
			{
				for(size_t x=0;x<actual->Width();++x)
				{
					if(actual->Value(x, y) != expected->Value(x, y))
					{
						if(errCount < 25)
						{
							if(errCount != 0) s << ", ";
							s << "sample (" << x << ',' << y << "), expected ";
							if(expected->Value(x,y)) s << "true"; else s << "false";
							s << ", actual ";
							if(actual->Value(x,y)) s << "true"; else s << "false";
						} else if(errCount == 25)
						{
							s << ", ...";
						}
						++errCount;
					} else if(actual->Value(x,y))
						trueMatches++;
					else falseMatches++;
				}
			}
			if(errCount != 0)
			{
				s << ". " << falseMatches << " matching false values, " << trueMatches << " matching true values, " << errCount << " errors.";
				throw std::runtime_error(s.str());
			}
		}
};

#endif
