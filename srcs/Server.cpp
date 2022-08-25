#include "Server.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

// https://cpp.hotexamples.com/fr/examples/-/-/FD_CLR/cpp-fd_clr-function-examples.html
// https://cpp.hotexamples.com/fr/site/file?hash=0x055b3d85737835dd5cefc2335071a6e3e792b28c40843fbd9ee1345c4679bfc6&fullName=apparmor-master/tests/regression/apparmor/test_multi_receive.c&project=crossbuild/apparmor
// Socket non bloquant

Server::Server(unsigned int port) {
	int fd;
	int nread;
	fd_set readfds, testfds;

	_socket.identifySocket(port);
	_socket.listenSocket();
	FD_ZERO(&readfds);
	FD_SET(_socket.getServerFd(), &readfds);
	while (1) {
		testfds = readfds;
		std::cerr << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
		select(FD_SETSIZE, &readfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0);
		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				if (fd == _socket.getServerFd()) {
					_socket.acceptSocket();
					FD_SET(_socket.getSocketFd(), &readfds);
					std::cerr << "\033[1;35mAdd client" << _socket.getSocketFd() << "\033[0m" << std::endl;
				}
			}
			else {
				ioctl(fd, FIONREAD, &nread);
				if (!nread) {
					close(fd);
					FD_CLR(fd, &readfds);
				}
				else {
					_request.fill(_socket.communicateSocket(fd));
					_response.respond(_request);
					write(fd, _response.send().c_str(), _response.send().size());
					std::cerr << "\033[1;35mServing client " << fd << "\033[0m" << std::endl;
				}
			}
		}
	}
}

Server::~Server() {}
