#include "Socket.hpp"

Socket::Socket() {
	int opt = true;

	// Socket domain and type
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		_perrorExit("cannot create socket");

	// Socket option to reuse our address
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		_perrorExit("cannot set socket option 'SO_REUSEADDR'");
}

Socket::~Socket(void) {
	close(_server_fd);
}

void signalHandler(int signal) {
	exit(signal);
}

void Socket::initialize(const std::string& address, unsigned int port) {
	signal(SIGINT, signalHandler);
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	inet_aton(address.c_str(), &_server_addr.sin_addr);
	// _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) 
		_perrorExit("bind failed"); 
	if (listen(_server_fd, SOMAXCONN) == -1) // Maximum number of client listned
		_perrorExit("listen failed");
	for (int i = 0; i < SOMAXCONN; i++)
		_client[i] = 0;
}

void Socket::waitRequest(void) {
	int maxfd = _server_fd;

	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_SET(_server_fd, &_readfds);
	FD_SET(_server_fd, &_writefds);
	for (int i = 0; i < SOMAXCONN; i++) {
		if (_client[i] > 0)
			FD_SET(_client[i], &_readfds);
		if (_client[i] > maxfd)
			maxfd = _client[i];
	}
	std::cerr << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
	if (select(maxfd + 1, &_readfds, &_writefds, NULL, NULL) == -1)
		_perrorExit("select failed");
}

void Socket::acceptClient(void) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addrlen = sizeof(sockaddr_in);

	if (FD_ISSET(_server_fd, &_readfds)) {
		if ((fd = accept(_server_fd, (struct sockaddr*)&addr, (socklen_t*)&addrlen)) == -1)
			_perrorExit("accept failed");
		for (int i = 0; i < SOMAXCONN; i++) {
			if (!_client[i]) {
				_client[i] = fd;
				std::cerr << "\033[1;35mAdd client " << fd << " " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;
				break;
			}
		}
	}
}

void Socket::communicate(void) {
	int rd = 0;
	char buffer[BUFFER_SIZE];
	Request request;
	Response response;

	for (int i = 0; i < SOMAXCONN; i++) {
		if (FD_ISSET(_client[i], &_readfds)) {
			if (!(rd = read(_client[i], buffer, BUFFER_SIZE))) {
				std::cerr << "\033[1;35mRemove client " << _client[i] << "\033[0m" << std::endl;
				close(_client[i]);
				_client[i] = 0;
			}
			else {
				buffer[rd] = '\0';
				request.fill(buffer);
				response.respond(request);
				send(_client[i], response.send().c_str(), response.send().size(), MSG_DONTWAIT);
			}
		}
	}
}

std::string Socket::getHeaderRequest(int fd) const {
	char buffer[BUFFER_SIZE];
	long rd = recv(fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);

	if (rd == -1)
		_perrorExit("recv failed");
	buffer[rd] = '\0';
	std::cerr << "\033[1;35mServing client " << fd << "\033[0m" << std::endl;
	return (buffer);
}

bool Socket::isReadSet(int fd) const {
	return (FD_ISSET(fd, &_readfds));
}

bool Socket::isWriteSet(int fd) const {
	return (FD_ISSET(fd, &_writefds));
}

int Socket::getServerFd(void) const {
	return (_server_fd);
}

void Socket::_perrorExit(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
