#include "Server.hpp"

Server::Server(const std::string& address, unsigned int port) {
	_socket.initialize(address, port);
	while (1) {
		_socket.waitRequest();
		for (Socket::map::iterator it = _socket.getClient().begin(); it != _socket.getClient().end(); it++) {
			if (FD_ISSET(it->first, _socket.getReadFds(1))) {
				if (it->first == _socket.getServerFd())
					_socket.acceptClient();
				else if (_socket.communicate(it)) {
					_request.fill(_socket.getHeaderRequest(it->first));
					_response.respond(_request);
					send(it->first, _response.send().c_str(), _response.send().size(), 0);
				}
				else
					it = _socket.getClient().begin();
			}
		}
	}
}

Server::~Server() {}
