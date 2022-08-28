#include "Server.hpp"

Server::Server(const std::string& address, unsigned int port) {
	_socket.initialize(address, port);
	while (1) {
		_socket.waitRequest();
		for (Socket::map::iterator it = _socket.getClient().begin(); it != _socket.getClient().end(); it++) {
			if (_socket.isReadSet(it->first)) { 
				if (it->first == _socket.getServerFd())
					_socket.acceptClient();
				else if (_socket.communicate(it->first)) {
					_request.fill(_socket.getHeaderRequest(it->first));
					_response.respond(_request);
					if (_socket.isWriteSet(it->first))
						send(it->first, _response.send().c_str(), _response.send().size(), MSG_DONTWAIT);
				}
				else
					it = _socket.getClient().begin();
			}
		}
	}
}

Server::~Server() {}
