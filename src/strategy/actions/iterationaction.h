#ifndef RFIITERATIONBLOCK_H
#define RFIITERATIONBLOCK_H 

#include <sstream>

#include "action.h"

#include "../control/artifactset.h"
#include "../control/actionblock.h"

#include "../../util/progresslistener.h"

namespace rfiStrategy {

	class IterationBlock : public ActionBlock
	{
		public:
			IterationBlock() : _iterationCount(4), _sensitivityStart(10.0L) { }
			virtual ~IterationBlock() { }

			virtual std::string Description()
			{
				std::stringstream s;
				s << "Iterate " << _iterationCount << " times";
				return s.str();
			}
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &listener)
			{
				long double oldSensitivity = artifacts.Sensitivity();

				long double sensitivityStep = powl(_sensitivityStart, 1.0L/_iterationCount);
				long double sensitivity = _sensitivityStart;

				for(size_t i=0;i<_iterationCount;++i)
				{
					artifacts.SetSensitivity(sensitivity * oldSensitivity);
					listener.OnStartTask(*this, i, _iterationCount, "Iteration");
					ActionBlock::Perform(artifacts, listener);
					listener.OnEndTask(*this);
					sensitivity /= sensitivityStep;
				}
				artifacts.SetSensitivity(oldSensitivity);
			}
			virtual ActionType Type() const { return IterationBlockType; }
			virtual unsigned int Weight() const { return ActionBlock::Weight() * _iterationCount; }

			size_t IterationCount() const throw() { return _iterationCount; }
			void SetIterationCount(size_t newCount) throw() { _iterationCount = newCount; }
			long double SensitivityStart() const throw() { return _sensitivityStart; }
			void SetSensitivityStart(long double sensitivityStart) throw() { _sensitivityStart = sensitivityStart; }
		private:
			size_t _iterationCount;
			long double _sensitivityStart;
	};

}

#endif // RFIITERATIONBLOCK_H
