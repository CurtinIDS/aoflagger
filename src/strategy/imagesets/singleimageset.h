#ifndef SINGLEIMAGESET_H
#define SINGLEIMAGESET_H

#include <string>
#include <stdexcept>

#include "../../structures/types.h"

#include "imageset.h"

namespace rfiStrategy {

	class SingleImageSetIndex : public ImageSetIndex {
		public:
			SingleImageSetIndex(ImageSet &set, std::string description) : ImageSetIndex(set), _valid(true), _description(description) { }
			virtual ~SingleImageSetIndex() { }
			virtual void Previous() { _valid = false; }
			virtual void Next() { _valid = false; }
			virtual std::string Description() const { return _description; }
			virtual bool IsValid() const { return _valid; }
			virtual ImageSetIndex *Copy() const
			{
				SingleImageSetIndex *index = new SingleImageSetIndex(imageSet(), _description);
				index->_valid = _valid;
				return index;
			}
		private:
			bool _valid;
			std::string _description;
	};
	
	class SingleImageSet : public ImageSet {
		public:
			SingleImageSet() : ImageSet(), _readCount(0), _lastRead(0), _writeFlagsIndex(0)
			{
			}
			
			virtual ~SingleImageSet()
			{
				delete _writeFlagsIndex;
				delete _lastRead;
			}

			virtual ImageSetIndex *StartIndex()
			{
				return new SingleImageSetIndex(*this, Name());
			}
			
			virtual std::string Name() = 0;
			virtual std::string File() = 0;
			
			virtual void AddReadRequest(const ImageSetIndex &)
			{
				if(_lastRead != 0)
				{
					delete _lastRead;
					_lastRead = 0;
					_readCount = 1;
				} else {
					++_readCount;
				}
			}
			virtual void PerformReadRequests()
			{
				_lastRead = Read();
				_lastRead->SetIndex(SingleImageSetIndex(*this, Name()));
			}
			virtual BaselineData *GetNextRequested()
			{
				if(_readCount == 0)
					throw std::runtime_error("All data reads have already been requested");
				if(_lastRead == 0)
					throw std::runtime_error("GetNextRequested() was called before PerformReadRequests()");
				return new BaselineData(*_lastRead);
			}
			
			virtual BaselineData *Read() = 0;
			
			virtual void Write(const std::vector<Mask2DCPtr>& flags)
			{
				throw std::runtime_error("Flag writing is not implemented for this file (SingleImageSet)");
			}
			
			virtual void AddWriteFlagsTask(const ImageSetIndex &index, std::vector<Mask2DCPtr> &flags)
			{
				delete _writeFlagsIndex;
				_writeFlagsIndex = index.Copy();
				_writeFlagsMasks = flags;
			}
			
			virtual void PerformWriteFlagsTask()
			{
				if(_writeFlagsIndex == 0)
					throw std::runtime_error("Nothing to write");
				
				Write(_writeFlagsMasks);
				
				delete _writeFlagsIndex;
				_writeFlagsIndex = 0;
			}
			
		private:
			int _readCount;
			BaselineData *_lastRead;
			ImageSetIndex *_writeFlagsIndex;
			std::vector<Mask2DCPtr> _writeFlagsMasks;
	};

}

#endif
