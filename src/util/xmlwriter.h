#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <fstream>
#include <string>
#include <stack>

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class XmlWriter {
	public:
		XmlWriter() : _file(0), _stream(0)
		{
		}

		~XmlWriter()
		{
			Close();
		}

		void Close()
		{
			if(_file != 0)
			{
				while(_openedElements.size() != 0)
					Close();
				delete _file;
				_file = 0;
			}
		}

		void StartDocument(const std::string &filename)
		{
			Close();
			_file = new std::ofstream(filename.c_str());
			_stream = _file;
			startDocument();
		}

		void StartDocument(std::ostream &stream)
		{
			Close();
			_stream = &stream;
			startDocument();
		}

		void Comment(const char *comment)
		{
			startNewTag();

			(*_stream) << "<!-- " << comment << "-->";
		}

		void Start(const char *element)
		{
			startNewTag();

			(*_stream) << '<' << element;
			_inTag = true;
			_tagHasData = false;
			_openedElements.push(element);
		}

		void End()
		{
			if(_openedElements.size() == 0)
				throw std::runtime_error("End() called without open element");
			if(_inTag)
				(*_stream) << " />";
			else {
				(*_stream) << '\n';
				indent(_openedElements.size()-1);
				(*_stream) << "</" << _openedElements.top() << '>';
			}
			_openedElements.pop();
			_inTag = false;
		}

		void Attribute(const char *attributeName, const char *value)
		{ 
			if(!_inTag)
				throw std::runtime_error("Attribute() called incorrectly");
			(*_stream) << ' ' << attributeName << "=\"" << value << "\"";
		}

		template<typename ValueType>
		void Attribute(const char *attributeName, ValueType value)
		{ 
			std::stringstream s;
			s << value;
			Attribute(attributeName, s.str().c_str());
		}

		void Write(const char *element, const char *value)
		{
			startNewTag();

			(*_stream) << '<' << element << '>' << value << "</" << element << '>';
		}

		template<typename ValueType>
		void Write(const char *element, ValueType value)
		{
			std::stringstream s;
			s << value;
			Write(element, s.str().c_str());
		}

	private:
		void startDocument()
		{
			(*_stream) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
			_inTag = false;
		}
		void closeTag()
		{
			if(_inTag)
			{
				(*_stream) << '>';
				_inTag = false;
			}
		}
		void startNewTag()
		{
			closeTag();
			(*_stream) << '\n';
			indent(_openedElements.size());
		}

		void indent(unsigned depth)
		{
			for(unsigned i=0;i<depth;++i)
				(*_stream) << ' ' << ' ';
		}

		std::stack<std::string> _openedElements;

		std::ofstream *_file;
		std::ostream *_stream;
		bool _inTag;
		bool _tagHasData;
};

#endif
