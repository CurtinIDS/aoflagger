#ifndef THRESHOLDCONFIG_H
#define THRESHOLDCONFIG_H

#include <vector>

#include "../../msio/image2d.h"
#include "../../msio/mask2d.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class ThresholdConfig {
	public:
		enum Method { SumThreshold, VarThreshold };
		enum Distribution { Uniform, Gaussian, Rayleigh };
		ThresholdConfig();
		~ThresholdConfig();
		void InitializeLengthsDefault(unsigned count=0);
		void InitializeLengthsFrom(unsigned count=0, unsigned startLength=0);
		void InitializeLengthsSingleSample();
		void InitializeThresholdsFromFirstThreshold(num_t firstThreshold, enum Distribution noiseDistribution);
		void InitializeThresholdsWithFalseRate(size_t resolution, num_t falseAlarmRate, enum Distribution noiseDistribution);
		num_t CalculateFalseAlarmRate(size_t resolution, enum Distribution noiseDistribution);
		void Execute(Image2DCPtr image, Mask2DPtr mask, bool additive, num_t sensitivity) const;
		void SetVerbose(bool verbose) { _verbose = verbose; }
		void SetMethod(Method method) { _method = method; }
		void SetExpThresholdFactor(num_t expFactor) { _expFactor=expFactor; }
		void SetFitBackground(class SurfaceFitMethod *method) { _fitMethod = method; }
		num_t GetHorizontalThreshold(unsigned index) const { return _horizontalOperations[index].threshold; }
		num_t GetVerticalThreshold(unsigned index) const { return _verticalOperations[index].threshold; }
		void SetHorizontalThreshold(unsigned index, num_t threshold) { _horizontalOperations[index].threshold = threshold; }
		void SetVerticalThreshold(unsigned index, num_t threshold) { _verticalOperations[index].threshold = threshold; }
		size_t GetHorizontalLength(unsigned index) const { return _horizontalOperations[index].length; }
		size_t GetVerticalLength(unsigned index) const { return _verticalOperations[index].length; }
		size_t GetHorizontalOperations() const { return _horizontalOperations.size(); }
		size_t GetVerticalOperations() const { return _horizontalOperations.size(); }
		void SetMinimumConnectedSamples(size_t val) { _minConnectedSamples = val; }
		void RemoveHorizontalOperations() { _horizontalOperations.clear(); }
		void RemoveVerticalOperations() { _verticalOperations.clear(); }
	private:
		void BinarySearch(num_t probability, num_t accuracy, size_t resolution);
		struct ThresholdOperation {
			size_t length;
			num_t threshold;
			num_t rate;
		};
		std::vector<ThresholdOperation> _horizontalOperations;
		std::vector<ThresholdOperation> _verticalOperations;
		Method _method;
		Distribution _distribution;
		bool _verbose;
		num_t _expFactor;
		class SurfaceFitMethod *_fitMethod;
		size_t _minConnectedSamples;
};

#endif
