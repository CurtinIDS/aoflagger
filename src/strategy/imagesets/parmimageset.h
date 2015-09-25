#ifndef PARM_IMAGE_H
#define PARM_IMAGE_H

#include <string>
#include <cstring>
#include <vector>
#include <deque>

#include "../../msio/types.h"
#include "../../msio/timefrequencydata.h"
#include "../../msio/timefrequencymetadata.h"

#include "imageset.h"

namespace rfiStrategy {

	class ParmImageSet;
	
	class ParmImageSetIndex : public ImageSetIndex {
		public:
			ParmImageSetIndex(ImageSet &set) : ImageSetIndex(set), _valid(true), _antennaIndex(0)
			{
			}
			virtual ~ParmImageSetIndex()
			{
			}
			inline virtual void Previous();
			
			inline virtual void Next();
			
			inline virtual std::string Description() const;
			
			virtual bool IsValid() const { return _valid; }
			
			virtual ParmImageSetIndex *Copy() const
			{
				ParmImageSetIndex *index = new ParmImageSetIndex(imageSet());
				index->_antennaIndex = _antennaIndex;
				return index;
			}
			
			unsigned AntennaIndex() const { return _antennaIndex; }
		private:
			inline ParmImageSet &ParmSet() const;
			bool _valid;
			unsigned _antennaIndex;
	};
	
	class ParmImageSet : public ImageSet {
		public:
			ParmImageSet(const std::string &path) : _path(path), _parmTable(0)
			{
			}
			virtual ~ParmImageSet();
			virtual ParmImageSet *Copy()
			{
				throw std::runtime_error("Can not copy set");
			}
			virtual ParmImageSetIndex *StartIndex()
			{
				return new ParmImageSetIndex(*this);
			}
			virtual void Initialize();
			
			virtual std::string Name() { return "Parmdb"; }
			
			virtual std::string File() { return _path; }
			
			virtual TimeFrequencyData *LoadData(const ImageSetIndex &index);
			
			virtual void AddReadRequest(const ImageSetIndex &index)
			{
				TimeFrequencyData *data = LoadData(index);
				BaselineData *baseline = new BaselineData(*data, TimeFrequencyMetaDataCPtr(), index);
				delete data;
				_baselineBuffer.push_back(baseline);
			}
			virtual void PerformReadRequests()
			{
			}
			
			virtual BaselineData *GetNextRequested()
			{
				BaselineData *baseline = _baselineBuffer.front();
				_baselineBuffer.pop_front();
				return baseline;
			}

			unsigned AntennaCount() const
			{
				return _antennas.size();
			}
			std::string AntennaName(unsigned index) const { return _antennas[index]; }
		private:
			const std::string _path;
			std::vector<std::string> _antennas;
			class ParmTable *_parmTable;
			std::deque<BaselineData*> _baselineBuffer;
	};

	void ParmImageSetIndex::Previous()
	{
		++_antennaIndex;
		if(_antennaIndex >= ParmSet().AntennaCount())
		{
			_antennaIndex = 0;
			_valid = false;
		}
	}
	
	void ParmImageSetIndex::Next()
	{
		if(_antennaIndex > 0)
			--_antennaIndex;
		else
		{
			_antennaIndex = ParmSet().AntennaCount() - 1;
			_valid = false;
		}
	}
	
	ParmImageSet &ParmImageSetIndex::ParmSet() const
	{
		return static_cast<ParmImageSet&>(imageSet());
	}

	std::string ParmImageSetIndex::Description() const
	{
		return ParmSet().AntennaName(_antennaIndex);
	}
}

#endif
