#include "../../util/progresslistener.h"

#include "statisticalflagaction.h"

#include "../algorithms/statisticalflagger.h"
#include "../algorithms/siroperator.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	void StatisticalFlagAction::Perform(ArtifactSet &artifacts, class ProgressListener &)
	{
		TimeFrequencyData &data = artifacts.ContaminatedData();
		if(data.MaskCount() > 1)
			throw std::runtime_error("Error: the statistical flag action (dilation operation) can only be applied on data with a single mask. Therefore, it should be placed 'under' a Set all polarization equal-operation, or inside a For each polarization action.");
			
		Mask2DPtr mask = Mask2D::CreateCopy(data.GetSingleMask());
		
		StatisticalFlagger::DilateFlags(mask, _enlargeTimeSize, _enlargeFrequencySize);
		//StatisticalFlagger::LineRemover(mask, (size_t) (_maxContaminatedTimesRatio * (double) mask->Width()), (size_t) (_maxContaminatedFrequenciesRatio * (double) mask->Height()));
		SIROperator::OperateHorizontally(mask, _minimumGoodTimeRatio);
		SIROperator::OperateVertically(mask, _minimumGoodFrequencyRatio);
		data.SetGlobalMask(mask);
		//artifacts.SetRevisedData(data);
	}

} // namespace rfiStrategy
