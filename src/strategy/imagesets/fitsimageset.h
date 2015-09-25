#ifndef FITSIMAGESET_H
#define FITSIMAGESET_H

#include <vector>
#include <set>
#include <stack>
#include <map>

#include "imageset.h"

#include "../../baseexception.h"

#include "../../msio/antennainfo.h"
#include "../../msio/types.h"

namespace rfiStrategy {
	
	class FitsImageSetIndex : public ImageSetIndex {
		friend class FitsImageSet;
		
		FitsImageSetIndex(class rfiStrategy::ImageSet &set) : ImageSetIndex(set), _baselineIndex(0), _band(0), _field(0), _isValid(true) { }
		
		virtual void Previous();
		virtual void Next();
		virtual std::string Description() const;
		virtual bool IsValid() const throw() { return _isValid; }
		virtual FitsImageSetIndex *Copy() const
		{
			FitsImageSetIndex *index = new FitsImageSetIndex(imageSet());
			index->_baselineIndex = _baselineIndex;
			index->_band = _band;
			index->_field = _field;
			index->_isValid = _isValid;
			return index;
		}
		private:
			size_t _baselineIndex, _band, _field;
			bool _isValid;
	};

	class FitsImageSet : public ImageSet
	{
		public:
			FitsImageSet(const std::string &file);
			~FitsImageSet();
			virtual void Initialize();

			virtual FitsImageSet *Copy();

			virtual ImageSetIndex *StartIndex()
			{
				return new FitsImageSetIndex(*this);
			}
			virtual std::string Name()
			{
				return File();
			}
			virtual std::string File();
			const std::vector<std::pair<size_t,size_t> > &Baselines() const throw() { return _baselines; }
			size_t BandCount() { return _bandCount; }
			class AntennaInfo GetAntennaInfo(unsigned antennaIndex) { return _antennaInfos[antennaIndex]; }
			virtual void AddReadRequest(const ImageSetIndex &index)
			{
				_baselineData.push(loadData(index));
			}
			virtual void PerformReadRequests()
			{
			}
			virtual BaselineData *GetNextRequested()
			{
				BaselineData *data = new BaselineData(_baselineData.top());
				_baselineData.pop();
				return data;
			}
			virtual void AddWriteFlagsTask(const ImageSetIndex &index, std::vector<Mask2DCPtr> &flags);
			virtual void PerformWriteFlagsTask();
			virtual void PerformWriteDataTask(const ImageSetIndex &, std::vector<Image2DCPtr>, std::vector<Image2DCPtr>)
			{
				throw BadUsageException("Not implemented");
			}
			
			std::string ReadTelescopeName();
			
		private:
			FitsImageSet(const FitsImageSet &source);
			BaselineData loadData(const ImageSetIndex &index);
			
			size_t getAntenna1(const ImageSetIndex &index) {
				return _baselines[static_cast<const FitsImageSetIndex&>(index)._baselineIndex].first;
			}
			size_t getAntenna2(const ImageSetIndex &index) {
				return _baselines[static_cast<const FitsImageSetIndex&>(index)._baselineIndex].second;
			}
			
			void ReadPrimarySingleTable(TimeFrequencyData &data, TimeFrequencyMetaData &metaData);
			void ReadTable(TimeFrequencyData &data, TimeFrequencyMetaData &metaData, size_t bandIndex);
			void ReadAntennaTable(TimeFrequencyMetaData &metaData);
			void ReadFrequencyTable(TimeFrequencyData &data, TimeFrequencyMetaData &metaData);
			void ReadCalibrationTable();
			void ReadSingleDishTable(TimeFrequencyData &data, TimeFrequencyMetaData &metaData, size_t ifIndex);
			TimeFrequencyData ReadPrimaryGroupTable(size_t baselineIndex, int band, int stokes, TimeFrequencyMetaData &metaData);
			
			void saveSingleDishFlags(std::vector<Mask2DCPtr> &flags, size_t ifIndex);
			
			boost::shared_ptr<class FitsFile> _file;
			std::vector<std::pair<size_t,size_t> > _baselines;
			size_t _bandCount;
			std::vector<AntennaInfo> _antennaInfos;
			std::map<int, BandInfo> _bandInfos;
			std::vector<int> _bandIndexToNumber;
			size_t _currentBaselineIndex, _currentBandIndex;
			double _frequencyOffset;
			
			std::stack<BaselineData> _baselineData;
	};

}

#endif
