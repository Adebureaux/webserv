#include "Server.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

Server::Server(unsigned int port) {
	fd_set rfds;

	_socket.identifySocket(port);
	while (1) {
		_socket.listenSocket();
		FD_ZERO(&rfds);
		FD_SET(_socket.getServerFd(), &rfds);
		if (select(_socket.getServerFd() + 1, &rfds, NULL, NULL, 0) == 1) {
			_socket.acceptSocket();
			_request.fill(_socket.communicateSocket());
			_response.respond(_request);
			write(_socket.getSocketFd(), _response.send().c_str(), _response.send().size());
			close(_socket.getSocketFd());
		}
	}
}

Server::~Server() {}
