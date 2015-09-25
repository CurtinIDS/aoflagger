#ifndef SPATIALCOMPOSITIONACTION_H
#define SPATIALCOMPOSITIONACTION_H

#include "action.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	/**
		@author A.R. Offringa <offringa@astro.rug.nl>
	*/
	class SpatialCompositionAction : public Action {
		public:
			enum Operation { SumCrossCorrelationsOperation, SumAutoCorrelationsOperation, EigenvalueDecompositionOperation, EigenvalueRemovalOperation } ;

			SpatialCompositionAction() : _operation(EigenvalueDecompositionOperation)
			{
			}
			virtual ~SpatialCompositionAction()
			{
			}
			virtual std::string Description()
			{
				switch(_operation)
				{
					default:
					case SumCrossCorrelationsOperation:
						return "Spatial composition (cross)";
					case SumAutoCorrelationsOperation:
						return "Spatial composition (auto)";
					case EigenvalueDecompositionOperation:
						return "Spatial composition (eigenvalue)";
					case EigenvalueRemovalOperation:
						return "Spatial composition (remove eigenvalue)";
				}
			}
			virtual void Perform(ArtifactSet &artifacts, ProgressListener &progress);

			virtual ActionType Type() const { return SpatialCompositionActionType; }

			enum Operation Operation() const { return _operation; }
			void SetOperation(enum Operation operation) { _operation = operation; }
		private:
			enum Operation _operation;

			num_t sumCrossCorrelations(Image2DCPtr image) const;
			num_t sumAutoCorrelations(Image2DCPtr image) const;
			num_t eigenvalue(Image2DCPtr real, Image2DCPtr imaginary) const;
			std::pair<num_t, num_t> removeEigenvalue(Image2DCPtr real, Image2DCPtr imaginary) const;
	};

}

#endif // SPATIALCOMPOSITIONACTION_H
