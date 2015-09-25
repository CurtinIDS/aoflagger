#ifndef SVDACTION_H
#define SVDACTION_H 

#include "action.h"

namespace rfiStrategy {

	class SVDAction : public Action
	{
		public:
			SVDAction() : _singularValueCount(1) { }
			virtual ~SVDAction() { }
			virtual std::string Description()
			{
				return "Singular value decomposition";
			}
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &listener);
			virtual ActionType Type() const { return SVDActionType; }

			size_t SingularValueCount() const throw() { return _singularValueCount; }
			void SetSingularValueCount(size_t svCount) throw() { _singularValueCount = svCount; }
		private:
			size_t _singularValueCount;
	};

}

#endif // SVDACTION_H
