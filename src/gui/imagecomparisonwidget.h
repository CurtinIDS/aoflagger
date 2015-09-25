#ifndef IMAGECOMPARISONWIDGET_H
#define IMAGECOMPARISONWIDGET_H

#include <gtkmm/drawingarea.h>

#include <vector>

#include "../msio/image2d.h"
#include "../msio/timefrequencydata.h"
#include "../msio/timefrequencymetadata.h"
#include "../msio/segmentedimage.h"

#include "imagewidget.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class ImageComparisonWidget : public ImageWidget {
	public:
		enum TFImage { TFOriginalImage, TFRevisedImage, TFContaminatedImage };
		ImageComparisonWidget();
		~ImageComparisonWidget();
		void SetNewData(const class TimeFrequencyData &image, TimeFrequencyMetaDataCPtr metaData);

		TimeFrequencyData GetActiveData() const
		{
			TimeFrequencyData data(getActiveDataWithOriginalFlags());
			data.SetGlobalMask(GetActiveMask());
			if(StartHorizontal() != 0.0 || EndHorizontal() != 1.0 || StartVertical() != 0.0 || EndVertical() != 1.0)
			   data.Trim(round(StartHorizontal() * data.ImageWidth()), round(StartVertical() * data.ImageHeight()),
									 round(EndHorizontal() * data.ImageWidth()), round(EndVertical() * data.ImageHeight())); 
			return data;
		}

		TimeFrequencyData &OriginalData() { return _original; }
		const TimeFrequencyData &OriginalData() const { return _original; }

		TimeFrequencyData &RevisedData() { return _revised; }
		const TimeFrequencyData &RevisedData() const { return _revised; }

		void SetRevisedData(const TimeFrequencyData &data)
		{
			_revised = data;
		  updateVisualizedImage();
		}
		const TimeFrequencyData &ContaminatedData() const { return _contaminated; }
		TimeFrequencyData &ContaminatedData() { return _contaminated; }
		void SetContaminatedData(const TimeFrequencyData &data)
		{
			_contaminated = data;
			SetAlternativeMask(data.GetSingleMask());
		  updateVisualizedImage();
		} 
		void SetVisualizedImage(TFImage visualizedImage)
		{
		  _visualizedImage = visualizedImage;
		  updateVisualizedImage();
		}
		void ClearBackground();
	private:
		void updateVisualizedImage();
		const TimeFrequencyData getActiveDataWithOriginalFlags() const
		{
			switch(_visualizedImage)
			{
				case TFOriginalImage:
				default:
					return _original;
				case TFRevisedImage:
					return _revised;
				case TFContaminatedImage:
					return _contaminated;
			}
		}
		enum TFImage _visualizedImage;
		TimeFrequencyData _original, _revised, _contaminated;
		TimeFrequencyMetaDataCPtr _metaData;
};

#endif
