#ifndef FILTERBANKSET_H
#define FILTERBANKSET_H

#include <string>

#include "imageset.h"

#include "../../util/aologger.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/

namespace rfiStrategy {

	class FilterBankSetIndex : public ImageSetIndex {
		public:
			friend class FilterBankSet;
			
			FilterBankSetIndex(class rfiStrategy::ImageSet &set) :
				ImageSetIndex(set), _intervalIndex(0), _isValid(true)
			{ }
			
			virtual void Previous();
			virtual void Next();
			virtual std::string Description() const;
			virtual bool IsValid() const { return _isValid; }
			virtual FilterBankSetIndex *Copy() const
			{
				FilterBankSetIndex *index = new FilterBankSetIndex(imageSet());
				index->_intervalIndex = _intervalIndex;
				index->_isValid = _isValid;
				return index;
			}
		private:
			size_t _intervalIndex;
			bool _isValid;
	};
	
	class FilterBankSet : public ImageSet {
		public:
			FilterBankSet(const std::string &location);
			
			~FilterBankSet()
			{ }

			virtual FilterBankSet* Copy()
			{
				return new FilterBankSet(_location);
			}
	
			virtual std::string Name() { return _location; }
			
			virtual std::string File() { return _location; }
			
			virtual void AddReadRequest(const ImageSetIndex &index);
			
			virtual void PerformReadRequests();
			
			virtual BaselineData *GetNextRequested();

			virtual void AddWriteFlagsTask(const ImageSetIndex &index, std::vector<Mask2DCPtr> &flags);
			
			virtual void PerformWriteFlagsTask();

			virtual void Initialize();
	
			virtual ImageSetIndex* StartIndex() { return new FilterBankSetIndex(*this); }

			virtual void WriteFlags(const ImageSetIndex &index, TimeFrequencyData &data);
			
			virtual void PerformWriteDataTask(const ImageSetIndex &index, std::vector<Image2DCPtr> realImages, std::vector<Image2DCPtr> imaginaryImages);
			
		private:
			friend class FilterBankSetIndex;
			std::string _location;
			
			double _timeOfSample, _timeStart, _fch1, _foff;
			size_t _channelCount, _ifCount, _bitCount, _sampleCount;
			size_t _machineId;
			
			static int32_t readInt(std::istream& str)
			{
				int32_t val;
				str.read(reinterpret_cast<char*>(&val), sizeof(int32_t));
				return val;
			}
			
			static double readDouble(std::istream& str)
			{
				double val;
				str.read(reinterpret_cast<char*>(&val), sizeof(double));
				return val;
			}
			
			static std::string readString(std::istream& str)
			{
				int32_t length = readInt(str);
				if(length <= 1 || length >= 80)
					return std::string();
				std::string data(length, 0);
				str.read(&data[0], length);
				return std::string(&data[0]);
			}
	};

}
	
#endif
