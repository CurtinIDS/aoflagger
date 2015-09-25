#ifndef INTEGERDOMAIN_H
#define INTEGERDOMAIN_H

#include <vector>
#include <string>

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class IntegerDomain {
	public:
		IntegerDomain(const IntegerDomain &source);
		IntegerDomain(int singleValue);
		IntegerDomain(const int *values, unsigned count);
		IntegerDomain(int first, unsigned count);
		IntegerDomain(int first, unsigned step, unsigned count);
		explicit IntegerDomain(const std::string &str);
		~IntegerDomain() { }
		unsigned ValueCount() const throw() { return _values.size(); }
		int GetValue(unsigned index) const throw() { return _values[index]; }
		void Join(const IntegerDomain &other) throw();
		bool IsIn(int number) const throw() {
			for(std::vector<int>::const_iterator i=_values.begin();	i!=_values.end();++i) {
				if(*i == number) return true;
			}
			return false;
		}
		IntegerDomain Split(unsigned partCount, unsigned partIndex) const;
		unsigned Index(int number) const throw() {
			for(unsigned i=0;	i!=_values.size();++i) {
				if(_values[i] == number) return i;
			}
			return unsigned(-1);
		}
		std::vector<int>::const_iterator begin() const { return _values.begin(); }
		std::vector<int>::const_iterator end() const { return _values.end(); }
	private:
		std::vector<int> _values;
};

#endif
