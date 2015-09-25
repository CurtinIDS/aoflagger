#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdexcept>

template<typename T>
class Parameter {
	public:
		Parameter() : _isSet(false), _value() { }
		Parameter(const T val) : _isSet(true), _value(val) { }
		Parameter(const Parameter<T> &source)
			: _isSet(source._isSet), _value(source._value) { }

		Parameter &operator=(const Parameter<T> &source)
		{
			_isSet = source._isSet;
			_value = source._value;
		}

		Parameter &operator=(T val)
		{
			_isSet = true;
			_value = val;
			return *this;
		}
		bool IsSet() const { return _isSet; }

		operator T() const
		{
			return Value();
		}

		T Value() const
		{
			if(_isSet)
				return _value;
			else
				throw std::runtime_error("Trying to access unset parameter");
		}

		T Value(T defaultValue) const
		{
			if(_isSet)
				return _value;
			else
				return defaultValue;
		}
	private:
		bool _isSet;
		T _value;
};

#endif //PARAMETER_H
