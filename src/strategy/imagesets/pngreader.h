#ifndef PNGREADER_H
#define PNGREADER_H

#include <string>
#include <fstream>
#include <set>
#include <map>
#include <cmath>

#include "../../msio/types.h"

#include "singleimageset.h"

#include "../../util/aologger.h"

namespace rfiStrategy {

	class PngReader : public SingleImageSet {
		public:
			PngReader(const std::string &path) : SingleImageSet(), _path(path)
			{
			}

			virtual ImageSet *Copy()
			{
				return 0;
			}

			virtual void Initialize()
			{
			}

			virtual std::string Name()
			{
				return "Png format";
			}
			
			virtual std::string File()
			{
				return _path;
			}
			
			virtual BaselineData *Read();

		private:
			std::string _path;
	};
	
}

#endif
