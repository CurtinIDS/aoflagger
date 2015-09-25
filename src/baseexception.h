/** @file
 * This is the header file for some of the exception classes and for the base exception class BaseException.
 * @author André Offringa <offringa@gmail.com>
 */
#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H

#include <exception>
#include <string>

/**
 * The base exception class that all exceptions should inherit from.
 */
class BaseException : public std::exception {
	public:
		/**
		 * Constructor that initialises the exception as if an unspecified error occured.
		 */
		BaseException() throw() : _description("Unspecified error") { }
		
		/**
		 * Constructor that initialises the exception with a specified description.
		 * @param description The description that should describe the cause of the exception.
		 */
		BaseException(const std::string &description) throw() : _description(description) { }
		
		/**
		 * Destructor.
		 */
		virtual ~BaseException() throw() { }
		
		/**
		 * Implementation of the std::exception::what() method that returns a description of the exception.
		 * @return A description of the exception.
		 */
		virtual const char *what() const throw() { return _description.c_str(); }
	private:
		const std::string _description;
};

/**
 * Exception that is throwed in case of Input/Output exceptions.
 */
class IOException : public BaseException {
	public:
		/**
		 * Constructor that initialises the IOException without a description.
		 */
		IOException() throw() : BaseException() { }
		
		/**
		 * Constructor that initialises the IOException with a description
		 * @param description The description of the Input/Output exception
		 */
		IOException(const std::string &description) throw() : BaseException(description) { }
};

/**
 * Exception that is throwed in case of an incorrect configuration
 */
class ConfigurationException : public BaseException {
	public:
		/**
		 * Constructor that initialises the ConfigurationException with a description
		 * @param description The description of the configuration error
		 */
		ConfigurationException(const std::string &description) throw() : BaseException(description) { }
};

/**
 * Exception that is throwed in case of an incorrect usage of a function
 */
class BadUsageException : public BaseException {
	public:
		/**
		 * Constructor that initialises the BadUsageException with a description
		 * @param description The description of the incorrect usage
		 */
		BadUsageException(const std::string &description) throw() : BaseException(description) { }
};
#endif
