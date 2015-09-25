#include <iostream>

#include "remote/client.h"

int main(int argc, char *argv[])
{
	if(argc == 3)
	{
		std::string serverHost = argv[2];
		aoRemote::Client client;
		client.Run(serverHost);
	} else {
		std::cerr << "Syntax: aoremoteclient connect <serverhost>\n";
	}
}
