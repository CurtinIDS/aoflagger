#include "actionblock.h"

#include "../../util/progresslistener.h"

namespace rfiStrategy {

	void ActionBlock::Perform(ArtifactSet &artifacts, ProgressListener &listener)
	{
		size_t nr = 0;
		unsigned totalWeight = Weight();
		for(const_iterator i=begin();i!=end();++i)
		{
			Action *action = *i;
			unsigned weight = action->Weight();
			listener.OnStartTask(*this, nr, totalWeight, action->Description(), weight);
			action->Perform(artifacts, listener);
			listener.OnEndTask(*this);
			nr += weight;
		}
	}
}
