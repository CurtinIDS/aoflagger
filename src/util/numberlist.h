#ifndef NUMBERLIST_H
#define NUMBERLIST_H

#include <cstdlib>
#include <string>
#include <vector>
#include <set>

class NumberList
{
public:
	template<typename IntType>
	static void ParseIntList(const std::string &str, std::vector<IntType>& list)
	{
		std::string temp = str;
		size_t pos = temp.find(",");
		while(pos != std::string::npos)
		{
			std::string idStr = temp.substr(0, pos);
			temp = temp.substr(pos+1);
			IntType num = atoi(idStr.c_str());
			list.push_back(num);
			pos = temp.find(",");
		}
		IntType num = atoi(temp.c_str());
		list.push_back(num);
	}
	
	template<typename IntType>
	static void ParseIntList(const std::string &str, std::set<IntType>& list)
	{
		std::string temp = str;
		size_t pos = temp.find(",");
		while(pos != std::string::npos)
		{
			std::string idStr = temp.substr(0, pos);
			temp = temp.substr(pos+1);
			IntType num = atoi(idStr.c_str());
			list.insert(num);
			pos = temp.find(",");
		}
		IntType num = atoi(temp.c_str());
		list.insert(num);
	}
};

#endif
