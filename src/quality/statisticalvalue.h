#ifndef MSIO_STATISTICAL_VALUE_H
#define MSIO_STATISTICAL_VALUE_H

#include <complex>

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/

class StatisticalValue {
	public:
		StatisticalValue(unsigned _polarizationCount) :
			_polarizationCount(_polarizationCount),
			_values(new std::complex<float>[_polarizationCount])
		{
		}
		
		StatisticalValue(const StatisticalValue &source) :
			_polarizationCount(source._polarizationCount),
			_values(new std::complex<float>[source._polarizationCount])
		{
			_kindIndex = source._kindIndex;
			for(unsigned i=0;i<_polarizationCount;++i)
				_values[i] = source._values[i];
		}
		
		~StatisticalValue()
		{
			delete[] _values;
		}
		
		StatisticalValue &operator=(const StatisticalValue &source)
		{
			if(_polarizationCount != source._polarizationCount)
			{
				_polarizationCount = source._polarizationCount;
				delete[] _values;
				_values = new std::complex<float>[_polarizationCount];
			}
			_kindIndex = source._kindIndex;
			for(unsigned i=0;i<_polarizationCount;++i)
				_values[i] = source._values[i];
			return *this;
		}
		
		unsigned PolarizationCount() const { return _polarizationCount; }
		
		unsigned KindIndex() const { return _kindIndex; }
		void SetKindIndex(unsigned kindIndex) { _kindIndex = kindIndex; }
		
		std::complex<float> Value(unsigned polarizationIndex) const { return _values[polarizationIndex]; }
		void SetValue(unsigned polarizationIndex, std::complex<float> newValue)
		{
			_values[polarizationIndex] = newValue;
		}
	
	private:
		unsigned _polarizationCount;
		unsigned _kindIndex;
		std::complex<float> *_values;
};

#endif
