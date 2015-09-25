#ifndef RFISTRATEGYIMAGERACTION_H
#define RFISTRATEGYIMAGERACTION_H

#include <boost/thread.hpp>

#include "action.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	/**
		@author A.R. Offringa <offringa@astro.rug.nl>
	*/
	class ImagerAction : public Action {
		public:
			enum ImagingType {
				Set, Add, Subtract
			};

			ImagerAction() : _type(Add)
			{
			}
			virtual ~ImagerAction()
			{
			}
			virtual std::string Description()
			{
				switch(_type)
				{
					case Set:
					return "Image (set)";
					case Add:
					default:
					return "Image (add)";
					case Subtract:
					return "Image (subtract)";
				}
			}
			virtual void Perform(ArtifactSet &artifacts, ProgressListener &progress);

			virtual ActionType Type() const { return ImagerActionType; }
			enum ImagingType ImagingType() const { return _type; }
			void SetImagingType(enum ImagingType type) throw() { _type = type; }

		private:
			enum ImagingType _type;
			boost::mutex _imagerMutex;
	};

}

#endif
