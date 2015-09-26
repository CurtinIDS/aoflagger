#ifndef LocalFitMethod_H
#define LocalFitMethod_H

#include <string>

#include <boost/thread/mutex.hpp>

#include "../../structures/image2d.h"
#include "../../structures/mask2d.h"
#include "../../structures/timefrequencydata.h"

#include "surfacefitmethod.h"

class LocalFitMethod : public SurfaceFitMethod {
	public:
		enum Method { None, Average, GaussianWeightedAverage, FastGaussianWeightedAverage, Median, Minimum };
		LocalFitMethod();
		~LocalFitMethod();
		void SetToAverage(unsigned hSquareSize, unsigned vSquareSize)
		{
			ClearWeights();
			_hSquareSize = hSquareSize;
			_vSquareSize = vSquareSize;
			_method = Average;
		}
		void SetToWeightedAverage(unsigned hSquareSize, unsigned vSquareSize, long double hKernelSize, long double vKernelSize)
		{
			ClearWeights();
			_hSquareSize = hSquareSize;
			_vSquareSize = vSquareSize;
			_method = FastGaussianWeightedAverage;
			_hKernelSize = hKernelSize;
			_vKernelSize = vKernelSize;
		}
		void SetToMedianFilter(unsigned hSquareSize, unsigned vSquareSize)
		{
			ClearWeights();
			_hSquareSize = hSquareSize;
			_vSquareSize = vSquareSize;
			_method = Median;
		}
		void SetToMinimumFilter(unsigned hSquareSize, unsigned vSquareSize)
		{
			ClearWeights();
			_hSquareSize = hSquareSize;
			_vSquareSize = vSquareSize;
			_method = Minimum;
		}
		void SetToNone()
		{
			ClearWeights();
			_method = None;
		}
		void SetParameters(unsigned hSquareSize, unsigned vSquareSize, enum Method method)
		{
			ClearWeights();
			_hSquareSize = hSquareSize;
			_vSquareSize = vSquareSize;
			_method = method;
		}
		virtual void Initialize(const TimeFrequencyData &input);
		virtual unsigned TaskCount();
		virtual void PerformFit(unsigned taskNumber);
		virtual TimeFrequencyData Background() { return *_background; }
		virtual enum TimeFrequencyData::PhaseRepresentation PhaseRepresentation() const
		{
			return TimeFrequencyData::AmplitudePart;
		}
	private:
		struct ThreadLocal {
			LocalFitMethod *image;
			unsigned currentX, currentY;
			unsigned startX, startY, endX, endY;
			size_t emptyWindows;
		};
		long double CalculateBackgroundValue(unsigned x, unsigned y);
		long double FitBackground(unsigned x, unsigned y, ThreadLocal &local);
		long double CalculateAverage(unsigned x, unsigned y, ThreadLocal &local);
		long double CalculateMedian(unsigned x, unsigned y, ThreadLocal &local);
		long double CalculateMinimum(unsigned x, unsigned y, ThreadLocal &local);
		long double CalculateWeightedAverage(unsigned x, unsigned y, ThreadLocal &local);
		void ClearWeights();
		void InitializeGaussianWeights();
		void PerformGaussianConvolution(Image2DPtr input);
		void CalculateWeightedAverageFast();
		Image2DPtr CreateFlagWeightsMatrix();
		void ElementWiseDivide(Image2DPtr leftHand, Image2DCPtr rightHand);

		Image2DCPtr _original;
		class TimeFrequencyData *_background;
		Image2DPtr _background2D;
		Mask2DCPtr _mask;
		unsigned _hSquareSize, _vSquareSize;
		num_t **_weights;
		long double _hKernelSize, _vKernelSize;
		boost::mutex _mutex;
		enum Method _method;
};

#endif
