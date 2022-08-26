#include "Server.hpp"

Server::Server(const std::string& address, unsigned int port) {
	int client_sockfd;
	int result;
	struct sockaddr_in client_address;
	unsigned int client_len;
	fd_set readfds, testfds;

	_socket.initialize(address, port);

	FD_ZERO(&readfds);
	FD_SET(_socket.getServerFd(), &readfds);

	_socket._client.insert(std::pair<int, sockaddr_in>(_socket.getServerFd(), _socket._server_addr));

	while (1) {
		std::map<int, sockaddr_in>::iterator it;
		int fd;
		int nread;

		testfds = readfds;
		std::cout << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
		result = select(_socket.getServerFd() + _socket._client.size(), &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);

		std::cout << result << " ret select = " << _socket._client.size() << std::endl;

		for (it = _socket._client.begin(); it != _socket._client.end(); it++) {
			fd = it->first;
			std::cout << "current client listening " << fd << std::endl;
			if (FD_ISSET(fd, &testfds)) {
				std::cout << "ENTER WITH FD = " << fd << std::endl;
				if (fd == _socket.getServerFd()) {
					client_len = sizeof(client_address);
					client_sockfd = accept(_socket.getServerFd(),
							(struct sockaddr *)&client_address, (socklen_t*)&client_len);
					FD_SET(client_sockfd, &readfds);
				} else {
					ioctl(fd, FIONREAD, &nread);
					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
					} else {
						_request.fill(_socket.getHeaderRequest(fd));
						_response.respond(_request);
						std::cerr << "\033[1;35mServing client " << fd << "\033[0m" << std::endl;
						send(fd, _response.send().c_str(), _response.send().size(), 0);
					}
				}
			}
	}

	}
}

Server::~Server() {}
