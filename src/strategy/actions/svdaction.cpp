#include "../../util/progresslistener.h"

#include "svdaction.h"

#include "../algorithms/svdmitigater.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	void SVDAction::Perform(ArtifactSet &artifacts, class ProgressListener &listener)
	{
		SVDMitigater mitigater;
		mitigater.Initialize(artifacts.ContaminatedData());
		mitigater.SetRemoveCount(_singularValueCount);
		for(size_t i=0;i<mitigater.TaskCount();++i)
		{
			mitigater.PerformFit(i);
			listener.OnProgress(*this, i+1, mitigater.TaskCount());
		}

		TimeFrequencyData newRevisedData = mitigater.Background();
		newRevisedData.SetMask(artifacts.RevisedData());

		TimeFrequencyData *contaminatedData =
			TimeFrequencyData::CreateTFDataFromDiff(artifacts.ContaminatedData(), newRevisedData);
		contaminatedData->SetMask(artifacts.ContaminatedData());

		artifacts.SetRevisedData(newRevisedData);
		artifacts.SetContaminatedData(*contaminatedData);

		delete contaminatedData;
	}

} // namespace rfiStrategy
