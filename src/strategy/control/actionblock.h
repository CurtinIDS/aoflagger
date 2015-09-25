#ifndef RFIACTIONBLOCK_H
#define RFIACTIONBLOCK_H

#include "../control/actioncontainer.h"

#include "../../util/types.h"

namespace rfiStrategy {

	class ActionBlock : public ActionContainer
	{
		public:
			virtual std::string Description()
			{
				return "Block";
			}
			virtual void Perform(class ArtifactSet &artifacts, ProgressListener &listener);

			virtual unsigned int Weight() const
			{
				unsigned int weight = 0;
				for(const_iterator i=begin();i!=end();++i)
				{
					weight += (*i)->Weight();
				}
				if(weight == 0)
					return 1;
				else
					return weight;
			}
	};
}

#endif // RFIACTIONBLOCK_H
