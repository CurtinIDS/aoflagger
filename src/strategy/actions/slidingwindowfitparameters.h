#include "../../msio/types.h"

#ifndef RFI_SLIDINGWINDOWFITPARAMETERS_H
#define RFI_SLIDINGWINDOWFITPARAMETERS_H

namespace rfiStrategy {

	struct SlidingWindowFitParameters
	{
		enum Method { None, Average, GaussianWeightedAverage, Median, Minimum };

		/**
		 * The method used for fitting.
		 */
		enum Method method;

		/**
		 * The window size in the time direction.
		 */
		size_t timeDirectionWindowSize;

		/**
		 * The window size in the frequency direction.
		 */
		size_t frequencyDirectionWindowSize;
		
		/**
		 * In the case of weighted average, the kernel size of the Gaussian function in the time direction.
		 */
		num_t timeDirectionKernelSize;

		/**
		 * In the case of weighted average, the kernel size of the Gaussian function in the frequency direction.
		 */
		num_t frequencyDirectionKernelSize;
	};
}

#endif // RFI_SLIDINGWINDOWFITPARAMETERS_H
