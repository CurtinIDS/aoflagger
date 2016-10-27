#ifndef RFIPLOTS_H
#define RFIPLOTS_H

#include <vector>

#include "../../structures/image2d.h"
#include "../../structures/mask2d.h"
#include "../../structures/samplerow.h"
#include "../../structures/timefrequencymetadata.h"

class RFIPlots{
	public:
		static void MakeDistPlot(class Plot2DPointSet &pointSet, Image2DCPtr image, Mask2DCPtr mask);
		template <bool Weight>
		static void MakeMeanSpectrumPlot(class Plot2DPointSet &pointSet, const TimeFrequencyData &data, const Mask2DCPtr &mask, const TimeFrequencyMetaDataCPtr &metaData);
		static void MakePowerSpectrumPlot(class Plot2DPointSet &pointSet, Image2DCPtr image, Mask2DCPtr mask, TimeFrequencyMetaDataCPtr metaData);
		static void MakeRMSSpectrumPlot(class Plot2DPointSet &plot, Image2DCPtr image, Mask2DCPtr mask);
		static void MakeSNRSpectrumPlot(class Plot2DPointSet &plot, Image2DCPtr image, Image2DCPtr model, Mask2DCPtr mask);
		static void MakePowerTimePlot(class Plot2DPointSet &plot, Image2DCPtr image, Mask2DCPtr mask, TimeFrequencyMetaDataCPtr metaData);
		static void MakeComplexPlanePlot(class Plot2DPointSet &plot, const class TimeFrequencyData &data, size_t xStart, size_t length, size_t y, size_t yAvgSize, Mask2DCPtr mask, bool realVersusImaginary, bool imaginary);
		static void MakeFittedComplexPlot(class Plot2DPointSet &plot, const TimeFrequencyData &data, size_t xStart, size_t length, size_t y, size_t yAvgSize, Mask2DCPtr mask, num_t sampleFringeFrequency, bool realVersusImaginary, bool imaginary);
		
		static void MakeTimeScatterPlot(class MultiPlot &plot, size_t plotIndex, Image2DCPtr image, Mask2DCPtr mask, TimeFrequencyMetaDataCPtr metaData);
		static void MakeTimeScatterPlot(class MultiPlot &plot, const TimeFrequencyData &data, TimeFrequencyMetaDataCPtr metaData, unsigned startIndex = 0);
		
		static void MakeFrequencyScatterPlot(class MultiPlot &plot, size_t plotIndex, Image2DCPtr image, Mask2DCPtr mask, TimeFrequencyMetaDataCPtr metaData);
		static void MakeFrequencyScatterPlot(class MultiPlot &plot, const TimeFrequencyData &data, TimeFrequencyMetaDataCPtr metaData, unsigned startIndex = 0);
		
		static void MakeScatterPlot(class MultiPlot &plot, size_t plotIndex, SampleRowCPtr row);
	private:
		/**
		 * Make a distribution curve for the provided image, ignoring masked values.
		 * @param image The image to make a distribution curve for, by binning
		 * @param mask Mask belonging to the image
		 * @param valuesOutput The number of pixels in the image that are in this bin
		 * @param binsOutput The ranges of the bins; the value in valuesOutput[i] is closer to value binsOutput[i] then
		 * it is to binsOutput[i-1] or binsOutput[i+1], i.e., valuesOutput[i] is about the average value in the bin.
		 * @param binCount The number of bins to make
		 * @param start Value that should be the limit for the lowest bin, start==end means use min
		 * @param end Value that should be the limit for the highest bin, start==end means use max
		 * @param factor A factor to scale the output (note that the outputs are integers)
		 * @param stretch A factor that is applied to all pixel-values before binning
		 */
		static void Bin(Image2DCPtr image, Mask2DCPtr mask, std::vector<size_t> &valuesOutput, std::vector<long double> &binsOutput, size_t binCount, long double start=0.0, long double end=0.0, long double factor=1.0, long double stretch=1.0) throw();

		RFIPlots() { }
		~RFIPlots() { }
};

#endif
