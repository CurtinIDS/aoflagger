#ifndef RFISLIDINGWINDOWFITACTION_H
#define RFISLIDINGWINDOWFITACTION_H 

#include "action.h"

#include "slidingwindowfitparameters.h"

namespace rfiStrategy {

	class SlidingWindowFitAction : public Action
	{
		public:
			SlidingWindowFitAction() { LoadDefaults(); }
			virtual ~SlidingWindowFitAction() { }
			virtual std::string Description()
			{
				return "Sliding window fit";
			}
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &listener);
			virtual ActionType Type() const { return SlidingWindowFitActionType; }

			const SlidingWindowFitParameters &Parameters() const throw() { return _parameters; }
			SlidingWindowFitParameters &Parameters() throw() { return _parameters; }

			void LoadDefaults();
		private:
			SlidingWindowFitParameters _parameters;
	};

}

#endif // RFISLIDINGWINDOWFITACTION_H
