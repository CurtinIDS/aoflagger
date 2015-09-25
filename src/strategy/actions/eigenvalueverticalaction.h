#ifndef EIGENVALUEVERTICALACTION_H
#define EIGENVALUEVERTICALACTION_H

#include "../../msio/timefrequencydata.h"

#include "action.h"

#include "../algorithms/vertevd.h"

#include "../control/artifactset.h"

namespace rfiStrategy {
	
	class EigenValueVerticalAction : public Action {
		public:
			EigenValueVerticalAction() : _timeIntegrated(true)
			{
			}
			~EigenValueVerticalAction()
			{
			}
			virtual std::string Description()
			{
				return "Eigen value decomposition (vertical)";
			}

			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &)
			{
				TimeFrequencyData &data = artifacts.ContaminatedData();
				if(data.PolarisationCount()!=1)
				{
					throw std::runtime_error("Eigen value decompisition requires one polarization");
				}
				VertEVD::Perform(data, _timeIntegrated);
			}

			virtual ActionType Type() const { return EigenValueVerticalActionType; }
		private:
			bool _timeIntegrated;
	};

}
	
#endif // EIGENVALUEVERTICALACTION_H
