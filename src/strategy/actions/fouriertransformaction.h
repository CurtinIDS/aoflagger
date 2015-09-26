#ifndef RFI_FOURIERTRANSFORM_ACTION_H
#define RFI_FOURIERTRANSFORM_ACTION_H

#include <iostream>

#include "../../util/ffttools.h"

#include "../../structures/timefrequencydata.h"

#include "action.h"

#include "../control/artifactset.h"
#include "../control/actionblock.h"

namespace rfiStrategy {

	class FourierTransformAction : public Action
	{
		public:
			FourierTransformAction() : Action(), _inverse(false), _dynamic(false), _sections(64)
			{
			}
			virtual std::string Description()
			{
				if(_inverse)
					return "Inv Fourier transform";
				else
					return "Fourier transform";
			}
			virtual ActionType Type() const { return FourierTransformActionType; }
			virtual void Perform(ArtifactSet &artifacts, class ProgressListener &)
			{
				perform(artifacts.ContaminatedData(), artifacts.MetaData());
			}
			bool Inverse() const { return _inverse; }
			void SetInverse(bool inverse) { _inverse = inverse; }
		private:
			void perform(TimeFrequencyData &data, TimeFrequencyMetaDataCPtr /*metaData*/)
			{
				if(data.PhaseRepresentation() != TimeFrequencyData::ComplexRepresentation || data.ImageCount() != 2)
					throw std::runtime_error("Fourier transform action needs a single complex image as input");
				Image2DPtr
					real = Image2D::CreateCopy(data.GetImage(0)),
					imaginary = Image2D::CreateCopy(data.GetImage(1));
				if(_dynamic)
				{
					FFTTools::CreateDynamicHorizontalFFTImage(real, imaginary, _sections, _inverse);
				} else {
					FFTTools::CreateHorizontalFFTImage(*real, *imaginary, _inverse);
				}
				data.SetImage(0, real);
				data.SetImage(1, imaginary);
			}

			bool _inverse;
			bool _dynamic;
			unsigned _sections;
	};

} // namespace

#endif // RFI_FOURIERTRANSFORM_ACTION_H
