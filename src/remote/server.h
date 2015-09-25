#ifndef AOREMOTE__SERVER_H
#define AOREMOTE__SERVER_H

#include "format.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/signals2/signal.hpp>

#include "serverconnection.h"

class StatisticsCollection;

namespace aoRemote {

/**
	* A server can listen for incoming connections on a TCP port. When incoming connections are created,
	* the class will spawn ServerConnection instances. All operations are asynchroneous.
	* @see ServerConnection
	*/
class Server
{
	public:
		/** After the Server has been constructed, incoming connections will no longer be refused. Only after
		 * Run() has been called, the connections will actually be accepted.
		 */
		Server();
		
		/**
		 * Open the server so that it will accept connections on the specific TCP port. Will not return
		 * untill the server has been stopped by calling Stop().
		 */
		void Run();
		
		/**
		 * Stop listening for connections. This will cause Run() to return. Note that this will not terminate
		 * any currently running connections; it will merely stop accepting new connections.
		 */
		void Stop();
		
		static unsigned PORT() { return 1892; }
		
		boost::signals2::signal<void(ServerConnectionPtr, bool&)> &SignalConnectionCreated()
		{
			return _onConnectionCreated;
		}
	private:
		void startAccept();
		void handleAccept(ServerConnectionPtr connection, const boost::system::error_code &error);
		
		boost::asio::io_service _ioService;
		boost::asio::ip::tcp::acceptor _acceptor;
		boost::signals2::signal<void(ServerConnectionPtr, bool&)> _onConnectionCreated;
};
	
}

#endif
