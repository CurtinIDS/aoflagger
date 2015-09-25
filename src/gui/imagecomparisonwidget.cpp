#include "imagecomparisonwidget.h"

#include "../strategy/algorithms/thresholdconfig.h"
#include "../strategy/algorithms/thresholdtools.h"

ImageComparisonWidget::ImageComparisonWidget() :
	_visualizedImage(TFOriginalImage)
{
}

ImageComparisonWidget::~ImageComparisonWidget()
{
}

void ImageComparisonWidget::SetNewData(const TimeFrequencyData &data, TimeFrequencyMetaDataCPtr metaData)
{
	Clear();
	
	_original = data;
	_revised = _original;
	_revised.SetImagesToZero();
	_contaminated = _original;
	updateVisualizedImage();
	
	SetOriginalMask(data.GetSingleMask());
	SetMetaData(metaData);
	ZoomFit();
}

void ImageComparisonWidget::updateVisualizedImage()
{
  Image2DCPtr image;
  switch(_visualizedImage)
	{
	case TFOriginalImage:
		if(_original.ContainsData())
			image = _original.GetSingleImage();
		break;
	case TFRevisedImage:
		if(_revised.ContainsData())
			image = _revised.GetSingleImage();
		break;
	case TFContaminatedImage:
		if(_contaminated.ContainsData())
			image = _contaminated.GetSingleImage();
		break;
	}
	if(image != 0)
		ImageWidget::SetImage(image);
} 

void ImageComparisonWidget::ClearBackground()
{
	_revised.SetImagesToZero();
}
