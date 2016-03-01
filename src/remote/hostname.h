#ifndef HOSTNAME_H
#define HOSTNAME_H

#include <string>

class Hostname
{
public:
	Hostname() : _hostname()
	{ }
	explicit Hostname(const std::string& str) : _hostname(str)
	{
		size_t dot = _hostname.find('.');
		if(dot == std::string::npos)
			_briefName = _hostname;
		else
			_briefName = _hostname.substr(0, dot);
	}
	
	const std::string& AsString() const { return _hostname; }
	
	bool operator<(const Hostname& rhs) { return _briefName < rhs._briefName; }
	bool operator==(const Hostname& rhs) { return _briefName == rhs._briefName; }
private:
	 std::string _hostname, _briefName;
};

inline bool operator<(const Hostname& lhs, const Hostname& rhs)
{ return lhs < rhs; }

#endif
