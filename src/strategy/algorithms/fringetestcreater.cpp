#include "fringetestcreater.h"

#include "../../msio/timefrequencydata.h"
#include "../../msio/timefrequencymetadata.h"

#include "../../imaging/uvimager.h"

void FringeTestCreater::AddStaticFringe(class TimeFrequencyData &ftData, TimeFrequencyMetaDataCPtr metaData, long double strength)
{
	Image2DCPtr
		real = ftData.GetRealPart(),
		imaginary = ftData.GetImaginaryPart();
	Image2DPtr
		newReal = Image2D::CreateUnsetImagePtr(real->Width(), real->Height()),
		newImaginary = Image2D::CreateUnsetImagePtr(real->Width(), real->Height());
	
	for(size_t channelIndex = 0; channelIndex < ftData.ImageHeight() ; ++channelIndex)
	{
		for(size_t t = 0; t < ftData.ImageWidth() ; ++t)
		{
			num_t fringeRotation =
				2.0 * M_PIn * UVImager::GetFringeCount(0, t, channelIndex, metaData);
			
			newReal->SetValue(t, channelIndex, strength * cosn(fringeRotation) + real->Value(t, channelIndex) );
			newImaginary->SetValue(t, channelIndex, strength * sinn(fringeRotation) + imaginary->Value(t, channelIndex) );
		}
	}
	
	ftData.Set(ftData.Polarisation(), newReal, newImaginary);
}
