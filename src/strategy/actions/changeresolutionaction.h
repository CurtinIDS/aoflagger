#ifndef CHANGERESOLUTIONACTION_H
#define CHANGERESOLUTIONACTION_H

#include "../../msio/timefrequencydata.h"

#include "../control/actionblock.h"

namespace rfiStrategy {
	
	class ChangeResolutionAction : public ActionBlock {
		public:
			ChangeResolutionAction() : _timeDecreaseFactor(10), _frequencyDecreaseFactor(1), _restoreRevised(true), _restoreContaminated(false), _restoreMasks(false), _useMaskInAveraging(false)
			{
			}
			~ChangeResolutionAction()
			{
			}
			virtual std::string Description()
			{
				return "Change resolution";
			}

			void SetTimeDecreaseFactor(int newFactor) { _timeDecreaseFactor = newFactor; }
			int TimeDecreaseFactor() const { return _timeDecreaseFactor; }

			void SetFrequencyDecreaseFactor(int newFactor) { _frequencyDecreaseFactor = newFactor; }
			int FrequencyDecreaseFactor() const { return _frequencyDecreaseFactor; }

			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &listener);
			virtual ActionType Type() const { return ChangeResolutionActionType; }

			bool RestoreRevised() const { return _restoreRevised; }
			void SetRestoreRevised(bool restoreRevised) { _restoreRevised = restoreRevised; }

			bool RestoreContaminated() const { return _restoreContaminated; }
			void SetRestoreContaminated(bool restoreContaminated) { _restoreContaminated = restoreContaminated; }

			bool RestoreMasks() const { return _restoreMasks; }
			void SetRestoreMasks(bool restoreMasks) { _restoreMasks = restoreMasks; }
			
			bool UseMaskInAveraging() const { return _useMaskInAveraging; }
			void SetUseMaskInAveraging(bool useMask) { _useMaskInAveraging = useMask; }
		private:
			void PerformFrequencyChange(class ArtifactSet &artifacts, class ProgressListener &listener);

			int _timeDecreaseFactor;
			int _frequencyDecreaseFactor;

			void DecreaseTime(TimeFrequencyData &data);
			void IncreaseTime(TimeFrequencyData &originalData, TimeFrequencyData &changedData, bool restoreImage, bool restoreMask);

			void DecreaseFrequency(TimeFrequencyData &data);
			void IncreaseFrequency(TimeFrequencyData &originalData, TimeFrequencyData &changedData, bool restoreImage, bool restoreMask);
			
			void DecreaseTimeWithMask(TimeFrequencyData &data);

			/**
			 * If this is true, the subtasks of this task can change the revised image, and
			 * this task will scale the changed images back to the original format. If it
			 * is false, the images before this task will be restored (which is faster, because
			 * then the changed image does not need to be enlarged).
			 */
			bool _restoreRevised;

			bool _restoreContaminated;

			/**
			 * This is like _restoreRevised, but for the masks.
			 */
			bool _restoreMasks;
			
			/**
			 * If set, values which are partially flagged, will be averaged only over
			 * non-flagged samples. This only changes the decreasing of resolution.
			 */
			bool _useMaskInAveraging;
	};

}
	
#endif
