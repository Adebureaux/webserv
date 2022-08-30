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
	// _client.insert(std::pair<int, sockaddr_in>(_server_fd, _server_addr));
}

void Socket::waitRequest(void) {
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_SET(_server_fd, &_readfds);
	FD_SET(_server_fd, &_writefds);
	for (map::iterator it = _client.begin(); it != _client.end(); it++) {
		FD_SET(it->first, &_readfds);
		// FD_SET(it->first, &_writefds);
	}
	std::cout << "select on " << _server_fd + _client.size() + 1 << std::endl;
	std::cerr << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
	if (select(_server_fd + _client.size() + 1, &_readfds, &_writefds, NULL, NULL) == -1)
		_perrorExit("select failed");
}

void Socket::acceptClient(void) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addrlen = sizeof(sockaddr_in);

	if (FD_ISSET(_server_fd, &_readfds)) {
		if ((fd = accept(_server_fd, (struct sockaddr*)&addr, (socklen_t*)&addrlen)) == -1)
			_perrorExit("accept failed");
		std::cerr << "\033[1;35mAdd client " << fd << " " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl << std::endl;
		_client.insert(std::pair<int, sockaddr_in>(fd, addr));
	}
}

void Socket::communicate(void) {
	int rd = 0;
	char buffer[BUFFER_SIZE];
	Request request;
	Response response;

	for (map::iterator it = _client.begin(); it != _client.end(); it++) {
		if (FD_ISSET(it->first, &_readfds)) {
			ioctl(it->first, FIONREAD, &rd);
			if (!rd) {
				std::cerr << "\033[1;35mRemove client " << it->first << " " << inet_ntoa(it->second.sin_addr) << ":" <<  ntohs(it->second.sin_port) << "\033[0m" << std::endl;
				close(it->first);
				FD_CLR(it->first, &_readfds);
				_client.erase(it->first);
				it = _client.begin();
			}
		}
		// else if (FD_ISSET(_server_fd, &_writefds)) {
		else {
			getHeaderRequest(it->first);
			request.fill(buffer);
			response.respond(request);
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

Socket::map& Socket::getClient(void) {
	return (_client);
}

int Socket::getServerFd(void) const {
	return (_server_fd);
}

void Socket::_perrorExit(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
