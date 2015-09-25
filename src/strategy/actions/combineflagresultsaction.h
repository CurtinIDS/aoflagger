#ifndef COMBINE_FLAG_RESULTS_H
#define COMBINE_FLAG_RESULTS_H

#include "../../util/progresslistener.h"

#include "../control/artifactset.h"
#include "../control/actioncontainer.h"

namespace rfiStrategy {

	class CombineFlagResults : public ActionContainer
	{
			public:
				virtual std::string Description()
				{
					return "Combine flag results";
				}
				virtual ActionType Type() const { return CombineFlagResultsType; }
				virtual void Perform(ArtifactSet &artifacts, class ProgressListener &listener)
				{
					if(GetChildCount() == 1)
					{
						GetFirstChild().Perform(artifacts, listener);
					} else {
						TimeFrequencyData originalFlags = artifacts.ContaminatedData();
						TimeFrequencyData joinedFlags = artifacts.ContaminatedData();
						size_t nr = 0;
						for(const_iterator i=begin();i!=end();++i)
						{
							artifacts.SetContaminatedData(originalFlags);
							Action *action = *i;
							listener.OnStartTask(*this, nr, GetChildCount(), action->Description());
							action->Perform(artifacts, listener);
							listener.OnEndTask(*this);
							++nr;
	
							joinedFlags.JoinMask(artifacts.ContaminatedData());
						}
						artifacts.SetContaminatedData(joinedFlags);
					}
				}
	};

} // namespace

#endif //COMBINE_FLAG_RESULTS_H
