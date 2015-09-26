#ifndef RFI_UV_PROJECT_ACTION_H
#define RFI_UV_PROJECT_ACTION_H

#include "action.h"

#include "../algorithms/uvprojection.h"

#include "../control/artifactset.h"
#include "../control/actionblock.h"

#include "../../structures/timefrequencydata.h"

#include <iostream>

namespace rfiStrategy {

	class UVProjectAction : public Action
	{
		public:
			UVProjectAction() : Action(), _directionRad(68.0/180.0*M_PI/*atann(600.0/(8.0*320.0))*/), _etaParameter(0.2), _reverse(false), _onRevised(false), _onContaminated(true)
			{
			}
			virtual std::string Description()
			{
				if(_reverse)
					return "Reverse UV-project";
				else
					return "UV-project";
			}
			virtual ActionType Type() const { return UVProjectActionType; }
			virtual void Perform(ArtifactSet &artifacts, class ProgressListener &)
			{
				if(_onContaminated)
					perform(artifacts.ContaminatedData(), artifacts.MetaData());
				if(_onRevised)
					perform(artifacts.RevisedData(), artifacts.MetaData());
				artifacts.SetProjectedDirectionRad(_directionRad);
			}
			numl_t DirectionRad() const { return _directionRad; }
			void SetDirectionRad(numl_t directionRad) { _directionRad = directionRad; }
			
			numl_t EtaParameter() const { return _etaParameter; }
			void SetEtaParameter(numl_t etaParameter) { _etaParameter = etaParameter; }

			num_t DestResolutionFactor() const { return _destResolutionFactor; }
			void SetDestResolutionFactor(num_t destResolutionFactor) { _destResolutionFactor = destResolutionFactor; }
			
			bool Reverse() const { return _reverse; }
			void SetReverse(bool reverse) { _reverse = reverse; }
			
			bool OnRevised() const { return _onRevised; }
			void SetOnRevised(bool onRevised) { _onRevised = onRevised; }
			
			bool OnContaminated() const { return _onContaminated; }
			void SetOnContaminated(bool onContaminated) { _onContaminated = onContaminated; }
		private:
			void perform(TimeFrequencyData &data, TimeFrequencyMetaDataCPtr metaData)
			{
				if(data.ImageCount()!=1)
					throw std::runtime_error("UV Projection can be applied on single images only");
				Image2DCPtr image = data.GetImage(0);

				Image2DPtr destination = Image2D::CreateZeroImagePtr(image->Width(), image->Height());
					
				if(_reverse)
				{
					UVProjection::InverseProjectImage(image, destination, metaData, _directionRad, _etaParameter, data.IsImaginary());
				} else
				{
					Image2DPtr weights = Image2D::CreateZeroImagePtr(image->Width(), image->Height());
				
					UVProjection::ProjectImage(image, destination, weights, metaData, _directionRad, _etaParameter, data.IsImaginary());
				}
				
				data.SetImage(0, destination);
			}
			
			numl_t _directionRad, _etaParameter;
			num_t _destResolutionFactor;
			bool _reverse;
			bool _onRevised, _onContaminated;
	};

} // namespace

#endif // RFI_UV_PROJECT_ACTION_H
