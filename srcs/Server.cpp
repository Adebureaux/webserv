#include "Server.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

Server::Server(unsigned int port) {
	_socket.identifySocket(port);
	_socket.listenSocket();
	while (1)
	{
		_socket.acceptSocket();
		_request.fill(_socket.communicateSocket());
		_response.respond(_request);
		write(_socket.getSocketFd(), _response.send().c_str(), _response.send().size());
	}
}

Server::~Server() {}
