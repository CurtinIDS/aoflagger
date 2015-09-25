#ifndef HIGHPASSFILTERACTION_H
#define HIGHPASSFILTERACTION_H

#include "action.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	/**
		@author A.R. Offringa <offringa@astro.rug.nl>
	*/
	class HighPassFilterAction : public Action {
		public:
			enum Mode { StoreContaminated, StoreRevised };
			HighPassFilterAction() :
				_windowWidth(22),
				_windowHeight(45),
				_hKernelSigmaSq(7.5),
				_vKernelSigmaSq(15.0),
				_mode(StoreContaminated)
			{
			}
			virtual ~HighPassFilterAction()
			{
			}
			virtual std::string Description()
			{
				return "High-pass filter (Gaussian)";
			}
			virtual void Perform(ArtifactSet &artifacts, ProgressListener &progress);

			virtual ActionType Type() const { return HighPassFilterActionType; }
			
			unsigned WindowWidth() const { return _windowWidth; }
			unsigned WindowHeight() const { return _windowHeight; }
			double HKernelSigmaSq() const { return _hKernelSigmaSq; }
			double VKernelSigmaSq() const { return _vKernelSigmaSq; }
			enum Mode Mode() const { return _mode; }
			
			void SetWindowWidth(unsigned width) { _windowWidth = width; }
			void SetWindowHeight(unsigned height) { _windowHeight = height; }
			void SetHKernelSigmaSq(double hSigmaSquared) { _hKernelSigmaSq = hSigmaSquared; }
			void SetVKernelSigmaSq(double vSigmaSquared) { _vKernelSigmaSq = vSigmaSquared; }
			void SetMode(enum Mode mode) { _mode = mode; }

		private:
			unsigned _windowWidth, _windowHeight;
			double _hKernelSigmaSq, _vKernelSigmaSq;
			enum Mode _mode;
	};

}

#endif
