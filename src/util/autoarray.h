#ifndef AUTO_ARRAY_H
#define AUTO_ARRAY_H

template<typename T>
class AutoArray
{
	public:
		AutoArray(T *arrayObject) :
			_arr(arrayObject)
		{
		}
		
		~AutoArray()
		{
			delete[] _arr;
		}
	
		T *operator->() const
		{
			return _arr;
		}
		
		T &operator*() const
		{
			return *_arr;
		}
	private:
		T *_arr;
};

#endif
