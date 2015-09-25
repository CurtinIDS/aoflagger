#include "server.h"

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

#include "serverconnection.h"

#include <iostream>

namespace aoRemote
{

Server::Server()
	: _acceptor(_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT()))
{
}

void Server::Run()
{
	startAccept();
	_ioService.run();
	_ioService.reset();
}

void Server::startAccept()
{
	ServerConnectionPtr connection = ServerConnection::Create(_ioService);
	
	_acceptor.async_accept(connection->Socket(), boost::bind(&Server::handleAccept, this, connection, boost::asio::placeholders::error));
}

void Server::handleAccept(ServerConnectionPtr connection, const boost::system::error_code &error)
{
	if (_acceptor.is_open())
	{
		if (!error)
		{
			std::cout << "Connection accepted." << std::endl;
			bool acceptConnection = true;
			_onConnectionCreated(connection, acceptConnection);
			if(acceptConnection)
				connection->Start();
		}

		startAccept();
	}
}

void Server::Stop()
{
	if(_acceptor.is_open())
		std::cout << "No longer accepting connections." << std::endl;
	_acceptor.close();
}


}
