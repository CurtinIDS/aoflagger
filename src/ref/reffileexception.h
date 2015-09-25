#ifndef AO_REFFILEEXCEPTION_H
#define AO_REFFILEEXCEPTION_H

#include <stdexcept>

namespace AOTools
{
	class RefFileException : public std::runtime_error
	{
		public:
		RefFileException() : std::runtime_error("Exception in reference file") { }
		RefFileException(const std::string &message) : std::runtime_error(message) { }
	};
}

#endif
