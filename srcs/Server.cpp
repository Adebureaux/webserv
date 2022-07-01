#include "Server.hpp"

Server::Server() {
	_socket.identify();
	_socket.listenSocket();
	_socket.acceptSocket();
	_socket.communicate();
}

Server::~Server() {}
