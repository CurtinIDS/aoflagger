#ifndef WRITEDATAACTION_H
#define WRITEDATAACTION_H

#include <boost/thread/mutex.hpp>

#include "action.h"

#include "../control/artifactset.h"

#include "../imagesets/imageset.h"

namespace rfiStrategy {

	class WriteDataAction : public Action {
		public:
			WriteDataAction()
			{
			}

			virtual ~WriteDataAction()
			{
			}

			virtual std::string Description()
			{
				return "Write data to file";
			}

			virtual void Perform(class ArtifactSet &artifacts, ProgressListener &)
			{
				boost::mutex::scoped_lock lock(artifacts.IOMutex());
				ImageSet &set = *artifacts.ImageSet();
				set.PerformWriteDataTask(*artifacts.ImageSetIndex(), artifacts.RevisedData());
			}

			virtual ActionType Type() const
			{
				return WriteDataActionType;
			}

		private:
	};
}
#endif
