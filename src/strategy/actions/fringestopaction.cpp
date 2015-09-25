#include "../../util/progresslistener.h"

#include "fringestopaction.h"

#include "../algorithms/fringestoppingfitter.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	void FringeStopAction::Perform(ArtifactSet &artifacts, class ProgressListener &listener)
	{
		if(!artifacts.HasMetaData())
			throw BadUsageException("No meta data available for fringe stopping");
		if(!artifacts.MetaData()->HasBand())
			throw BadUsageException("Baseline band data not set");
		if(!artifacts.MetaData()->HasAntenna1() || !artifacts.MetaData()->HasAntenna2())
			throw BadUsageException("Baseline antenna info not set");
		if(!artifacts.MetaData()->HasObservationTimes())
			throw BadUsageException("Baseline observation times not set");

		FringeStoppingFitter fitter;
		fitter.SetFringesToConsider(_fringesToConsider);
		fitter.SetMinWindowSize(_minWindowSize);
		fitter.SetMaxWindowSize(_maxWindowSize);
		fitter.SetFitChannelsIndividually(_fitChannelsIndividually);
		fitter.SetMetaData(artifacts.MetaData());
		fitter.SetNewPhaseCentreRA(_newPhaseCentreRA);
		fitter.SetNewPhaseCentreDec(_newPhaseCentreDec);
		fitter.Initialize(artifacts.ContaminatedData());
		if(_onlyFringeStop)
			fitter.PerformFringeStop();
		else {
			for(size_t i=0;i<fitter.TaskCount();++i)
			{
				fitter.PerformFit(i);
				listener.OnProgress(*this, i+1, fitter.TaskCount());
			}
		}

		TimeFrequencyData newContaminatedData = fitter.Background();
		newContaminatedData.SetMask(artifacts.ContaminatedData());

		TimeFrequencyData *newRevisedData =
			TimeFrequencyData::CreateTFDataFromDiff(artifacts.OriginalData(), newContaminatedData);
		newRevisedData->SetMask(artifacts.RevisedData());

		artifacts.SetRevisedData(*newRevisedData);
		artifacts.SetContaminatedData(newContaminatedData);

		delete newRevisedData;
	}

} // namespace rfiStrategy
