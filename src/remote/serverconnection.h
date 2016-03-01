#ifndef AOREMOTE__SERVER_CONNECTION_H
#define AOREMOTE__SERVER_CONNECTION_H

#include <string>

#include <boost/asio/ip/tcp.hpp>

#include <boost/enable_shared_from_this.hpp>

#include <boost/signals2/signal.hpp>

#include "format.h"
#include "hostname.h"

#include "../structures/antennainfo.h"
#include "../structures/msrowdataext.h"

class StatisticsCollection;
class HistogramCollection;

namespace aoRemote {

typedef boost::shared_ptr<class ServerConnection> ServerConnectionPtr;

class ServerConnection : public boost::enable_shared_from_this<ServerConnection>

{
	public:
		static ServerConnectionPtr Create(boost::asio::io_service &ioService)
		{
			return ServerConnectionPtr(new ServerConnection(ioService));
		}
		~ServerConnection();
		
		void StopClient();
		void ReadQualityTables(const std::string &msFilename, class StatisticsCollection &collection, HistogramCollection &histogramCollection);
		void ReadAntennaTables(const std::string &msFilename, boost::shared_ptr<std::vector<AntennaInfo> > antennas);
		void ReadBandTable(const std::string &msFilename, BandInfo &band);
		void ReadDataRows(const std::string &msFilename, size_t rowStart, size_t rowCount, MSRowDataExt *destinationArray);
		void WriteDataRows(const std::string &msFilename, size_t rowStart, size_t rowCount, const MSRowDataExt *rowArray);
		void Start();
		
		boost::asio::ip::tcp::socket &Socket() { return _socket; }
		
		boost::signals2::signal<void(ServerConnectionPtr)> &SignalAwaitingCommand() { return _onAwaitingCommand; }
		boost::signals2::signal<void(ServerConnectionPtr, StatisticsCollection&, HistogramCollection&)> &SignalFinishReadQualityTables() { return _onFinishReadQualityTables; }
		boost::signals2::signal<void(ServerConnectionPtr, boost::shared_ptr<std::vector<AntennaInfo> >, size_t)> &SignalFinishReadAntennaTables() { return _onFinishReadAntennaTables; }
		boost::signals2::signal<void(ServerConnectionPtr, BandInfo&)> &SignalFinishReadBandTable() { return _onFinishReadBandTable; }
		boost::signals2::signal<void(ServerConnectionPtr, MSRowDataExt*, size_t)> &SignalFinishReadDataRows() { return _onFinishReadDataRows; }
		boost::signals2::signal<void(ServerConnectionPtr, const std::string&)> &SignalError() { return _onError; }
		
		const Hostname &GetHostname() const { return _hostname; }
	private:
		ServerConnection(boost::asio::io_service &ioService);
		boost::asio::ip::tcp::socket _socket;
		Hostname _hostname;
		
		boost::signals2::signal<void(ServerConnectionPtr)> _onAwaitingCommand;
		boost::signals2::signal<void(ServerConnectionPtr, StatisticsCollection&, HistogramCollection&)> _onFinishReadQualityTables;
		boost::signals2::signal<void(ServerConnectionPtr, boost::shared_ptr<std::vector<AntennaInfo> >, size_t)> _onFinishReadAntennaTables;
		boost::signals2::signal<void(ServerConnectionPtr, BandInfo&)> _onFinishReadBandTable;
		boost::signals2::signal<void(ServerConnectionPtr, MSRowDataExt*, size_t)> _onFinishReadDataRows;
		boost::signals2::signal<void(ServerConnectionPtr, const std::string&)> _onError;
		
		char *_buffer;
		
		void onReceiveInitialResponse();
		
		void onReceiveQualityTablesResponseHeader();
		void onReceiveQualityTablesResponseData(size_t dataSize);
		
		void onReceiveAntennaTablesResponseHeader();
		void onReceiveAntennaTablesResponseData(size_t dataSize);
		
		void onReceiveBandTableResponseHeader();
		void onReceiveBandTableResponseData(size_t dataSize);
		
		void onReceiveReadDataRowsResponseHeader();
		void onReceiveReadDataRowsResponseData(size_t dataSize);
		
		void onReceiveWriteDataRowsResponseHeader();
		
		void prepareBuffer(size_t size)
		{
			if(_buffer != 0) delete[] _buffer;
			_buffer = new char[size];
		}
		
		void handleError(const GenericReadResponseHeader &header);
		
		StatisticsCollection *_collection;
		HistogramCollection *_histogramCollection;
		boost::shared_ptr<std::vector<AntennaInfo> > _antennas;
		BandInfo *_band;
		MSRowDataExt *_readRowData;
		const MSRowDataExt *_writeRowData;
};
	
}

#endif
