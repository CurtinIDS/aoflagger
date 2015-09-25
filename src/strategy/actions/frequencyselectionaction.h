#ifndef FREQUENCYSELECTIONACTION_H
#define FREQUENCYSELECTIONACTION_H

#include "../../msio/types.h"

#include "action.h"

namespace rfiStrategy {
	
	class FrequencySelectionAction : public Action {
		public:
			FrequencySelectionAction() : _threshold(3.0), _clipDown(true)
			{
			}
			~FrequencySelectionAction()
			{
			}
			virtual std::string Description()
			{
				return "Frequency selection";
			}
			virtual void Perform(ArtifactSet &artifacts, class ProgressListener &listener);
			virtual ActionType Type() const { return FrequencySelectionActionType; }

			num_t Threshold() const { return _threshold; }
			void SetThreshold(num_t threshold) { _threshold = threshold; }

			bool ClipDown() const { return _clipDown; }
			void SetClipDown(bool clipDown) { _clipDown = clipDown; }
		private:
			double _threshold;
			bool _clipDown;
	};

}

#endif
