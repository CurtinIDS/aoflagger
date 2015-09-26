#ifndef TIMESELECTIONACTION_H
#define TIMESELECTIONACTION_H

#include "../../structures/types.h"

#include "action.h"

namespace rfiStrategy {
	
	class TimeSelectionAction : public Action {
		public:
			TimeSelectionAction() : _threshold(3.5)
			{
			}
			~TimeSelectionAction()
			{
			}
			virtual std::string Description()
			{
				return "Time selection";
			}
			virtual void Perform(ArtifactSet &artifacts, class ProgressListener &)
			{
				AutomaticSelection(artifacts);
			}
			virtual ActionType Type() const { return TimeSelectionActionType; }

			num_t Threshold() const { return _threshold; }
			void SetThreshold(num_t threshold) { _threshold = threshold; }
		private:
			void AutomaticSelection(ArtifactSet &artifacts);

			num_t _threshold;
	};

}

#endif
