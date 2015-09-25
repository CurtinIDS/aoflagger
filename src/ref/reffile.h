#ifndef AO_REFFILE_H
#define AO_REFFILE_H

#include <fstream>
#include <string>
#include <vector>

#include "reffileentry.h"
#include "reffileexception.h"

namespace AOTools
{
	class RefFile
	{
		public:
			typedef std::vector<RefFileEntry>::const_iterator const_iterator;

			RefFile()
			{
			}

			explicit RefFile(const std::string &refFilePath)
			{
				Read(refFilePath);
			}

			void Read(const std::string &refFilePath)
			{
				_entries.clear();
				_refFilePath = refFilePath;
				std::ifstream file(_refFilePath.c_str());
				RefFileEntry entry;
				while(entry.read(file))
				{
					_entries.push_back(entry);
				}
			}

			void Write(std::ostream &destination) const
			{
				for(const_iterator i=begin();i!=end();++i)
					i->write(destination);
			}

			size_t Count() const
			{
				return _entries.size();
			}

			const RefFileEntry &operator[](const size_t index) const
			{
				return _entries[index];
			}

			void Add(const RefFileEntry &entry)
			{
				_entries.push_back(entry);
			}
	
			const_iterator begin() const
			{
				return _entries.begin();
			}

			const_iterator end() const
			{
				return _entries.end();
			}

		private:
			RefFile(const RefFile &) // don't allow copy
			{
			}

			void operator=(const RefFile &) // don't allow assignment
			{
			}

			std::vector<RefFileEntry> _entries;
			std::string _refFilePath;
	};
}

#endif //AO_REFFILE_H
