#include "samplerow.h"

SampleRowPtr SampleRow::CreateWithoutMissings() const
{
	size_t newSize = 0;
	for(size_t i=0;i<_size;++i)
		if(std::isfinite(_values[i])) ++newSize;
	SampleRow *newRow = new SampleRow(newSize);
	size_t indexToNew = 0;
	for(size_t i=0;i<_size;++i)
	{
		if(std::isfinite(_values[i]))
		{
			newRow->_values[indexToNew] = _values[i];
			++indexToNew;
		}
	}
	return SampleRowPtr(newRow);
}
