#include "client.h"

#include <typeinfo>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <casacore/ms/MeasurementSets/MSColumns.h>

#include "../quality/histogramcollection.h"
#include "../quality/histogramtablesformatter.h"
#include "../quality/qualitytablesformatter.h"
#include "../quality/statisticscollection.h"

#include "format.h"
#include "processcommander.h"

#include "../structures/antennainfo.h"
#include "../structures/measurementset.h"

namespace aoRemote
{

Client::Client()
	: _socket(_ioService)
{
}

void Client::Run(const std::string &serverHost)
{
	boost::asio::ip::tcp::resolver resolver(_ioService);
	std::stringstream s;
	s << PORT();
	boost::asio::ip::tcp::resolver::query query(serverHost, s.str());
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
	
	boost::asio::ip::tcp::endpoint endpoint = *iter;
	_socket.connect(endpoint);
	
	const std::string hostname = ProcessCommander::GetHostName();
	struct InitialBlock initialBlock;
	boost::asio::read(_socket, boost::asio::buffer(&initialBlock, sizeof(initialBlock)));
	
	struct InitialResponseBlock initialResponse;
	initialResponse.blockIdentifier = InitialResponseId;
	initialResponse.blockSize = sizeof(initialResponse);
	initialResponse.negotiatedProtocolVersion = AO_REMOTE_PROTOCOL_VERSION;
	initialResponse.hostNameSize = hostname.size();
	if(initialBlock.protocolVersion != AO_REMOTE_PROTOCOL_VERSION || initialBlock.blockSize != sizeof(initialBlock) || initialBlock.blockIdentifier != InitialId)
	{
		initialResponse.errorCode = ProtocolNotUnderstoodError;
		boost::asio::write(_socket, boost::asio::buffer(&initialResponse, sizeof(initialResponse)));
		boost::asio::write(_socket, boost::asio::buffer(hostname));
		return;
	}
	initialResponse.errorCode = NoError;
	boost::asio::write(_socket, boost::asio::buffer(&initialResponse, sizeof(initialResponse)));
	boost::asio::write(_socket, boost::asio::buffer(hostname));

	while(true)
	{
		struct RequestBlock requestBlock;
		boost::asio::read(_socket, boost::asio::buffer(&requestBlock, sizeof(requestBlock)));
		
		enum RequestType request = (enum RequestType) requestBlock.request;
		switch(request)
		{
			case StopClientRequest:
				return;
			case ReadQualityTablesRequest:
				handleReadQualityTables(requestBlock.dataSize);
				break;
			case ReadAntennaTablesRequest:
				handleReadAntennaTables(requestBlock.dataSize);
				break;
			case ReadBandTableRequest:
				handleReadBandTable(requestBlock.dataSize);
				break;
			case ReadDataRowsRequest:
				handleReadDataRows(requestBlock.dataSize);
				break;
			case WriteDataRowsRequest:
				handleWriteDataRows(requestBlock.dataSize);
				break;
			default:
				std::cout << "CLIENT: unknown command sent" << std::endl;
				writeGenericReadException("Command not understood by client: server and client versions don't match?");
				break;
		}
	}
}

void Client::writeGenericReadException(const std::exception &e)
{
	std::stringstream s;
	s << "Exception type " << typeid(e).name() << ": " << e.what();
	writeGenericReadException(s.str());
}

void Client::writeGenericReadException(const std::string &s)
{
	GenericReadResponseHeader header;
	header.blockIdentifier = GenericReadResponseHeaderId;
	header.blockSize = sizeof(header);
	header.errorCode = UnexpectedExceptionOccured;
	header.dataSize = s.size();
	
	boost::asio::write(_socket, boost::asio::buffer(&header, sizeof(header)));
	boost::asio::write(_socket, boost::asio::buffer(s));
}

void Client::writeGenericReadError(enum ErrorCode error)
{
	GenericReadResponseHeader header;
	header.blockIdentifier = GenericReadResponseHeaderId;
	header.blockSize = sizeof(header);
	header.errorCode = CouldNotOpenTableError;
	header.dataSize = 0;
	boost::asio::write(_socket, boost::asio::buffer(&header, sizeof(header)));
}

void Client::writeDataResponse(std::ostringstream &buffer)
{
	try {
		GenericReadResponseHeader header;
		header.blockIdentifier = GenericReadResponseHeaderId;
		header.blockSize = sizeof(header);
		header.errorCode = NoError;
		const std::string str = buffer.str();
		header.dataSize = str.size();
		
		boost::asio::write(_socket, boost::asio::buffer(&header, sizeof(header)));
		if(str.size() != 0)
			boost::asio::write(_socket, boost::asio::buffer(str));
	} catch(std::exception &e) {
		writeGenericReadException(e);
	}
}

std::string Client::readStr(unsigned size)
{
	std::vector<char> data(size+1);
	boost::asio::read(_socket, boost::asio::buffer(&data[0], size));
	data[size] = 0;
	return std::string(&data[0]);
}

void Client::handleReadQualityTables(unsigned dataSize)
{
	try {
		ReadQualityTablesRequestOptions options;
		boost::asio::read(_socket, boost::asio::buffer(&options.flags, sizeof(options.flags)));
		
		unsigned nameLength = dataSize - sizeof(options.flags);
		options.msFilename = readStr(nameLength);
		
		QualityTablesFormatter formatter(options.msFilename);
		if(!formatter.TableExists(QualityTablesFormatter::KindNameTable))
		{
			writeGenericReadError(CouldNotOpenTableError);
		} else {
			StatisticsCollection collection(formatter.GetPolarizationCount());
			collection.Load(formatter);
			// TODO: maybe we want to configure the following parameter at one point
			collection.LowerTimeResolution(1000);
			
			HistogramTablesFormatter histogramFormatter(options.msFilename);
			HistogramCollection histogramCollection(formatter.GetPolarizationCount());
			const bool histogramsExist = histogramFormatter.HistogramsExist();
			if(histogramsExist)
			{
				histogramCollection.Load(histogramFormatter);
			}
			
			std::ostringstream s;
			collection.Serialize(s);
			if(histogramsExist)
				histogramCollection.Serialize(s);
			writeDataResponse(s);
		}
	} catch(std::exception &e) {
		writeGenericReadException(e);
	}
}

void Client::handleReadAntennaTables(unsigned dataSize)
{
	try {
		ReadAntennaTablesRequestOptions options;
		boost::asio::read(_socket, boost::asio::buffer(&options.flags, sizeof(options.flags)));
		
		unsigned nameLength = dataSize - sizeof(options.flags);
		options.msFilename = readStr(nameLength);
		
		std::ostringstream buffer;
		
		// Serialize the antennae info
		MeasurementSet ms(options.msFilename);
		size_t polarizationCount = ms.PolarizationCount();
		size_t antennas = ms.AntennaCount();
		Serializable::SerializeToUInt32(buffer, polarizationCount);
		Serializable::SerializeToUInt32(buffer, antennas);
		for(unsigned aIndex = 0; aIndex<antennas; ++aIndex)
		{
			AntennaInfo antennaInfo = ms.GetAntennaInfo(aIndex);
			antennaInfo.Serialize(buffer);
		}
		
		writeDataResponse(buffer);
	} catch(std::exception &e) {
		writeGenericReadException(e);
	}
}

void Client::handleReadBandTable(unsigned dataSize)
{
	try {
		ReadBandTableRequestOptions options;
		boost::asio::read(_socket, boost::asio::buffer(&options.flags, sizeof(options.flags)));
		
		unsigned nameLength = dataSize - sizeof(options.flags);
		options.msFilename = readStr(nameLength);
		
		std::ostringstream buffer;
		
		// Serialize the band info
		MeasurementSet ms(options.msFilename);
		if(ms.BandCount() != 1)
			throw std::runtime_error("The number of bands in the measurement set was not 1");
		BandInfo band = ms.GetBandInfo(0);
		band.Serialize(buffer);
		
		writeDataResponse(buffer);
	} catch(std::exception &e) {
		writeGenericReadException(e);
	}
}

void Client::handleReadDataRows(unsigned dataSize)
{
	try {
		ReadDataRowsRequestOptions options;
		boost::asio::read(_socket, boost::asio::buffer(&options.flags, sizeof(options.flags)));
		
		unsigned nameLength = dataSize - sizeof(options.flags) - sizeof(options.startRow) - sizeof(options.rowCount);
		options.msFilename = readStr(nameLength);
		
		boost::asio::read(_socket, boost::asio::buffer(&options.startRow, sizeof(options.startRow)));
		boost::asio::read(_socket, boost::asio::buffer(&options.rowCount, sizeof(options.rowCount)));
		
		std::ostringstream buffer;
		Serializable::SerializeToUInt64(buffer, options.rowCount);
		
		// Read meta data from the MS
		casacore::Table table(options.msFilename);
		if(options.rowCount == 0)
			Serializable::SerializeToUInt64(buffer, table.nrow());
		else {
			casacore::ROArrayColumn<casacore::Complex> dataCol(table, "DATA");
			casacore::ROArrayColumn<double> uvwColumn(table, "UVW");
			casacore::ROScalarColumn<int> a1Column(table, "ANTENNA1");
			casacore::ROScalarColumn<int> a2Column(table, "ANTENNA2");
			casacore::ROScalarColumn<double> timeColumn(table, "TIME");
			const casacore::IPosition &shape = dataCol.shape(0);
			size_t channelCount, polarizationCount;
			if(shape.nelements() > 1)
			{
				channelCount = shape[1];
				polarizationCount = shape[0];
			}
			else
				throw std::runtime_error("Unknown shape of DATA column");
			const size_t samplesPerRow = polarizationCount * channelCount;
			
			// Read and serialize the rows
			const size_t endRow = options.startRow + options.rowCount;
			for(size_t rowIndex=options.startRow; rowIndex != endRow; ++rowIndex)
			{
				// DATA
				const casacore::Array<casacore::Complex> cellData = dataCol(rowIndex);
				casacore::Array<casacore::Complex>::const_iterator cellIter = cellData.begin();
				
				MSRowDataExt dataExt(polarizationCount, channelCount);
				MSRowData &data = dataExt.Data();
				num_t *realPtr = data.RealPtr();
				num_t *imagPtr = data.ImagPtr();
				for(size_t i=0;i<samplesPerRow;++i) {
					*realPtr = cellIter->real();
					*imagPtr = cellIter->imag();
					++realPtr;
					++imagPtr;
					++cellIter;
				}
				
				// UVW
				casacore::Array<double> uvwArr = uvwColumn(rowIndex);
				casacore::Array<double>::const_iterator uvwIter = uvwArr.begin();
				dataExt.SetU(*uvwIter);
				++uvwIter;
				dataExt.SetV(*uvwIter);
				++uvwIter;
				dataExt.SetW(*uvwIter);
				
				// OTHER
				dataExt.SetAntenna1(a1Column(rowIndex));
				dataExt.SetAntenna2(a2Column(rowIndex));
				dataExt.SetTime(timeColumn(rowIndex));
				dataExt.SetTimeOffsetIndex(rowIndex);
				
				dataExt.Serialize(buffer);
			}
		}
		
		writeDataResponse(buffer);
	} catch(std::exception &e) {
		writeGenericReadException(e);
	}
}

void Client::handleWriteDataRows(unsigned dataSize)
{
	try {
		WriteDataRowsRequestOptions options;
		
		boost::asio::read(_socket, boost::asio::buffer(&options.flags, sizeof(options.flags)));
		unsigned nameLength = dataSize - sizeof(options.flags) - sizeof(options.startRow) - sizeof(options.rowCount) - sizeof(options.dataSize);
		options.msFilename = readStr(nameLength);
		boost::asio::read(_socket, boost::asio::buffer(&options.startRow, sizeof(options.startRow)));
		boost::asio::read(_socket, boost::asio::buffer(&options.rowCount, sizeof(options.rowCount)));
		boost::asio::read(_socket, boost::asio::buffer(&options.dataSize, sizeof(options.dataSize)));
		
		// Read the big data chunk
		std::vector<char> dataBuffer(options.dataSize);
		boost::asio::read(_socket, boost::asio::buffer(&dataBuffer[0], options.dataSize));
		std::istringstream stream;
		if(stream.rdbuf()->pubsetbuf(&dataBuffer[0], options.dataSize) == 0)
			throw std::runtime_error("Could not set string buffer");
		
		// Write the received data to the MS
		casacore::Table table(options.msFilename, casacore::Table::Update);
		casacore::ArrayColumn<casacore::Complex> dataCol(table, "DATA");
		//casacore::ROScalarColumn<int> a1Column(table, "ANTENNA1");
		//casacore::ROScalarColumn<int> a2Column(table, "ANTENNA2");
		const casacore::IPosition shape = dataCol.shape(0);
		size_t channelCount, polarizationCount;
		if(shape.nelements() > 1)
		{
			channelCount = shape[1];
			polarizationCount = shape[0];
		}
		else
			throw std::runtime_error("Unknown shape of DATA column");
		const size_t samplesPerRow = polarizationCount * channelCount;
		
		// Unserialize and write the rows
		casacore::Array<casacore::Complex> cellData(shape);
		const size_t endRow = options.startRow + options.rowCount;
		for(size_t rowIndex=options.startRow; rowIndex != endRow; ++rowIndex)
		{
			MSRowDataExt dataExt;
			dataExt.Unserialize(stream);
			MSRowData &data = dataExt.Data();
			
			casacore::Array<casacore::Complex>::iterator cellIter = cellData.begin();
			
			num_t *realPtr = data.RealPtr();
			num_t *imagPtr = data.ImagPtr();
			for(size_t i=0;i<samplesPerRow;++i) {
				*cellIter = casacore::Complex(*realPtr, *imagPtr);
				++realPtr;
				++imagPtr;
				++cellIter;
			}
			dataCol.put(rowIndex, cellData);
		}
		
		std::ostringstream buffer;
		writeDataResponse(buffer);
	} catch(std::exception &e) {
		writeGenericReadException(e);
	}
}

} // namespace

