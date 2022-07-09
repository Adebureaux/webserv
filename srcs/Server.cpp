#include "Server.hpp"

Server::Server(unsigned int port) {
	SimpleSocket socket;

	socket.identifySocket(port);
	socket.listenSocket();
	while (1)
	{
		socket.acceptSocket();
		socket.communicateSocket();
	}
}

Server::~Server() {}
