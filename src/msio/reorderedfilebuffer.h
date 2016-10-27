#ifndef REORDERED_FILE_BUFFER_H
#define REORDERED_FILE_BUFFER_H

#include <fstream>
#include <vector>
#include <set>

#include "../util/aologger.h"

class ReorderedFileBuffer
{
public:
	ReorderedFileBuffer(std::ofstream* stream, size_t maxSize) :
		_nextWritePos(0), _unflushedSize(0), _maxSize(maxSize), _stream(stream)
	{
	}
	
	~ReorderedFileBuffer()
	{
		flush();
	}
	
	void seekp(size_t offset)
	{
		_nextWritePos = offset;
	}
	
	void write(const char* data, size_t length)
	{
		_buffer.insert(BufferEntry(_nextWritePos, data, length));
		_nextWritePos += length;
		_unflushedSize += length + sizeof(_nextWritePos)*2;
		
		if(_unflushedSize > _maxSize)
			flush();
	}
	
	void flush()
	{
		AOLogger::Debug << "Flushing reordered file buffer...\n";
		for(std::set<BufferEntry>::const_iterator i=_buffer.begin(); i!=_buffer.end(); ++i)
		{
			_stream->seekp(i->position, std::ios_base::beg);
			_stream->write(i->data.data(), i->data.size());
			if(_stream->fail())
				throw std::runtime_error("Error: failed to write to reordered file! Check access rights and free disk space.");
		}
		_buffer.clear();
		_unflushedSize = 0;
	}
	
	std::ofstream& stream() { return *_stream; }
	
private:
	struct BufferEntry {
		BufferEntry(size_t pos_, const char* data_, size_t length_) :
			position(pos_),
			data(data_, data_ + length_)
		{ }
		bool operator<(const BufferEntry& other) const
		{
			return position < other.position;
		}
		size_t position;
		std::vector<char> data;
	};
	std::set<BufferEntry> _buffer;
	size_t _nextWritePos;
	size_t _unflushedSize;
	size_t _maxSize;
	std::ofstream* _stream;
};

#endif
