#include "../../util/progresslistener.h"

#include "slidingwindowfitaction.h"

#include "../algorithms/localfitmethod.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	void SlidingWindowFitAction::LoadDefaults()
	{
		_parameters.frequencyDirectionKernelSize = 15.0;
		_parameters.frequencyDirectionWindowSize = 40;
		_parameters.method = SlidingWindowFitParameters::GaussianWeightedAverage;
		_parameters.timeDirectionKernelSize = 7.5;
		_parameters.timeDirectionWindowSize = 20;
	}

	void SlidingWindowFitAction::Perform(ArtifactSet &artifacts, class ProgressListener &listener)
	{
		LocalFitMethod method;
		switch(_parameters.method)
		{
			case SlidingWindowFitParameters::None:
				method.SetToNone();
				break;
			case SlidingWindowFitParameters::Average:
				method.SetToAverage(
					_parameters.timeDirectionWindowSize,
					_parameters.frequencyDirectionWindowSize);
				break;
			case SlidingWindowFitParameters::GaussianWeightedAverage:
				method.SetToWeightedAverage(
					_parameters.timeDirectionWindowSize,
					_parameters.frequencyDirectionWindowSize,
					_parameters.timeDirectionKernelSize,
					_parameters.frequencyDirectionKernelSize);
				break;
			case SlidingWindowFitParameters::Median:
				method.SetToMedianFilter(
					_parameters.timeDirectionWindowSize,
					_parameters.frequencyDirectionWindowSize);
				break;
			case SlidingWindowFitParameters::Minimum:
				method.SetToMinimumFilter(
					_parameters.timeDirectionWindowSize,
					_parameters.frequencyDirectionWindowSize);
				break;
		}

		method.Initialize(artifacts.ContaminatedData());
		
		size_t taskCount = method.TaskCount();
		for(size_t i=0;i<taskCount;++i)
		{
			method.PerformFit(i);
			listener.OnProgress(*this, i+1, taskCount);
		}
		TimeFrequencyData newRevisedData = method.Background();
		newRevisedData.SetMask(artifacts.RevisedData());

		TimeFrequencyData *contaminatedData =
			TimeFrequencyData::CreateTFDataFromDiff(artifacts.ContaminatedData(), newRevisedData);
		contaminatedData->SetMask(artifacts.ContaminatedData());

		artifacts.SetRevisedData(newRevisedData);
		artifacts.SetContaminatedData(*contaminatedData);

		delete contaminatedData;
	}
} // namespace rfiStrategy
