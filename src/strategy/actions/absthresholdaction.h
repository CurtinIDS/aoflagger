#ifndef RFIABSTHRESHOLDACTION_H
#define RFIABSTHRESHOLDACTION_H

#include "../../util/progresslistener.h"

#include "../../msio/timefrequencydata.h"

#include "../control/actioncontainer.h"
#include "../control/artifactset.h"

namespace rfiStrategy {

	class AbsThresholdAction : public Action
	{
		public:
			AbsThresholdAction() : _threshold(1.0)
			{
			}
			
			virtual std::string Description()
			{
				return "Absolute threshold";
			}
			
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &)
			{
				TimeFrequencyData &data = artifacts.ContaminatedData();
				if(data.PolarisationCount() != 1)
					throw std::runtime_error("Can not threshold multiple polarisations at once; use a for-each-polarisation block.");
				
				data.SetGlobalMask(PerformThreshold(data.GetSingleImage()));
			}
			virtual ActionType Type() const { return AbsThresholdActionType; }
			
			double Threshold() const { return _threshold; }
			void SetThreshold(double threshold) { _threshold = threshold; }
		private:
			Mask2DPtr PerformThreshold(Image2DCPtr image) const
			{
				const unsigned
					width = image->Width(),
					height = image->Height();
				Mask2DPtr mask = Mask2D::CreateUnsetMaskPtr(width, height);
				for(unsigned y=0;y<height;++y)
				{
					for(unsigned x=0;x<width;++x)
					{
						const num_t value = image->Value(x, y);
						const bool flag = !std::isfinite(value) || fabsn(value)>_threshold;
						mask->SetValue(x, y, flag);
					}
				}
				return mask;
			}
			
			double _threshold;
	};
}

#endif // RFIADDSTATISTICSACTION_H
