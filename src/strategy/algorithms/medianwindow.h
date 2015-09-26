#ifndef MEDIANWINDOW_H
#define MEDIANWINDOW_H

#include <set>

#include "../../structures/samplerow.h"

template<typename NumType>
class MedianWindow
{
	public:
		void Add(NumType newSample)
		{
			_set.insert(newSample);
		}
		void Remove(NumType sample)
		{
			_set.erase(_set.find(sample));
		}
		NumType Median() const
		{
			if(_set.size() == 0)
				return std::numeric_limits<NumType>::quiet_NaN();
			if(_set.size() % 2 == 0)
			{
				unsigned m = _set.size() / 2 - 1;
				typename std::multiset<NumType>::const_iterator i = _set.begin();
				for(unsigned j=0;j<m;++j)
					++i;
				NumType lMid = *i;
				++i;
				NumType rMid = *i;
				return (lMid + rMid) / 2.0;
			} else {
				unsigned m = _set.size() / 2;
				typename std::multiset<NumType>::const_iterator i = _set.begin();
				for(unsigned j=0;j<m;++j)
					++i;
				return *i;
			}
		}
		static void SubtractMedian(SampleRowPtr sampleRow, unsigned windowSize)
		{
			if(windowSize > sampleRow->Size()*2)
				windowSize = sampleRow->Size()*2;
			SampleRowCPtr copy = SampleRow::CreateCopy(sampleRow);
			MedianWindow<num_t> window;
			unsigned rightPtr, leftPtr = 0;
			for(rightPtr=0;rightPtr<windowSize/2;++rightPtr)
			{
				if(!copy->ValueIsMissing(rightPtr))
					window.Add(copy->Value(rightPtr));
			}
			for(unsigned i=0;i<sampleRow->Size();++i)
			{
				if(rightPtr < sampleRow->Size())
				{
					if(!copy->ValueIsMissing(rightPtr))
						window.Add(copy->Value(rightPtr));
					++rightPtr;
				}
				if(rightPtr >= windowSize)
				{
					if(!copy->ValueIsMissing(leftPtr))
						window.Remove(copy->Value(leftPtr));
					++leftPtr;
				}
				sampleRow->SetValue(i, copy->Value(i) - window.Median());
			}
		}
	private:
		std::multiset<NumType> _set;
};

#endif
