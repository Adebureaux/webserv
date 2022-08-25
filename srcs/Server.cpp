#include "Server.hpp"

Server::Server(const std::string& address, unsigned int port) {
	_socket.initialize(address, port);

	// fd_set set;
	// FD_ZERO(&set);
	// FD_SET(_socket.getServerFd(), &set);
	// select(_socket.getServerFd() + 1, &set, NULL, NULL, NULL);

	while (1) {
		std::cout << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
		// if (_socket.selects() > 1)
		_socket.acceptClient();
		std::cout << "\033[1;35mAdd client " << _socket.getClientFd() << "\033[0m" << std::endl;
		_request.fill(_socket.getHeaderRequest());
		_response.respond(_request);
		std::cout << "\033[1;35mServing client " << _socket.getClientFd() << "\033[0m" << std::endl;
		write(_socket.getClientFd(), _response.send().c_str(), _response.send().size());

	}
}

Server::~Server() {}
