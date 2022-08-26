#include "Server.hpp"

Server::Server(const std::string& address, unsigned int port) {
	int rd;

	_socket.initialize(address, port);
	while (1) {
		_socket.waitRequest();
		for (std::map<int, sockaddr_in>::iterator it = _socket._client.begin(); it != _socket._client.end(); it++) {
			if (FD_ISSET(it->first, _socket.getReadFds(1))) {
				if (it->first == _socket.getServerFd())
					_socket.acceptClient();
				else {
					ioctl(it->first, FIONREAD, &rd);
					if (!rd) {
						printf("removing client on fd %d\n", it->first);
						close(it->first);
						FD_CLR(it->first, &_socket._read_fds[0]);
						_socket._client.erase(it->first);
						it = _socket._client.begin();
					}
					else 
					{
						std::cerr << "\033[1;35mServing client " << it->first << "\033[0m" << std::endl;
						_request.fill(_socket.getHeaderRequest(it->first));
						_response.respond(_request);
						send(it->first, _response.send().c_str(), _response.send().size(), 0);
					}
				}
			}
		}
	}
}

Server::~Server() {}
