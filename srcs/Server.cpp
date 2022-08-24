#include "Server.hpp"

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
