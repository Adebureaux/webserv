#include "Server.hpp"

Server::Server(const std::string& address, unsigned int port) {
	int rd;
	int client_sockfd;

	_socket.initialize(address, port);
	FD_ZERO(&_socket._read_fds);
	FD_SET(_socket.getServerFd(), &_socket._read_fds);
	_socket._client.insert(std::pair<int, sockaddr_in>(_socket.getServerFd(), _socket._server_addr));
	while (1) {
		_socket._test_fds = _socket._read_fds;
		std::cout << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
		select(_socket.getServerFd() + _socket._client.size(), &_socket._test_fds, (fd_set *)0, (fd_set *)0, (struct timeval*)0);
		for (std::map<int, sockaddr_in>::iterator it = _socket._client.begin(); it != _socket._client.end(); it++) {
			std::cout << "current client listening " << it->first << std::endl;
			if (FD_ISSET(it->first, &_socket._test_fds)) {
				if (it->first == _socket.getServerFd()) {
					client_sockfd = _socket.acceptClient();
					FD_SET(client_sockfd, &_socket._read_fds);
				}
				else {
					ioctl(it->first, FIONREAD, &rd);
					if (!rd) {
						close(it->first);
						FD_CLR(it->first, &_socket._read_fds);
						_socket._client.erase(it->first);
						it = _socket._client.begin();
						printf("removing client on fd %d\n", it->first);
					} else {
						_request.fill(_socket.getHeaderRequest(it->first));
						_response.respond(_request);
						std::cerr << "\033[1;35mServing client " << it->first << "\033[0m" << std::endl;
						send(it->first, _response.send().c_str(), _response.send().size(), 0);
					}
				}
			}
	}

	}
}

Server::~Server() {}
