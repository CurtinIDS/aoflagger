#ifndef AOREMOTE__CLIENT_H
#define AOREMOTE__CLIENT_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "format.h"

namespace aoRemote {

class Client
{
	public:
		Client();
		
		void Run(const std::string &serverHost);
		
		static unsigned PORT() { return 1892; }
		
	private:
		boost::asio::io_service _ioService;
		boost::asio::ip::tcp::socket _socket;
		
		void writeGenericReadException(const std::exception &e);
		void writeGenericReadException(const std::string &s);
		void writeGenericReadError(enum ErrorCode code);
		void writeDataResponse(std::ostringstream &buffer);
		
		std::string readStr(unsigned size);
		
		void handleReadQualityTables(unsigned dataSize);
		void handleReadAntennaTables(unsigned dataSize);
		void handleReadBandTable(unsigned dataSize);
		void handleReadDataRows(unsigned dataSize);
		void handleWriteDataRows(unsigned dataSize);
};
	
}

#endif
