#include "Socket.hpp"

Socket::Socket() {
	int opt = 1;

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

void Socket::initialize(const std::string& address, unsigned int port) {
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	inet_aton(address.c_str(), &_server_addr.sin_addr);
	// _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// fcntl(_server_fd, F_SETFL, O_NONBLOCK); // SETUP SELECT FIRST
	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) 
		_perrorExit("bind failed"); 
	if (listen(_server_fd, 100) == -1) // Maximum number of client listned
		_perrorExit("listen failed");
	FD_ZERO(&_read_fds[0]);
	FD_SET(_server_fd, &_read_fds[0]);
	_client.insert(std::pair<int, sockaddr_in>(_server_fd, _server_addr));
}

void Socket::waitRequest(void) {
	_read_fds[1] = _read_fds[0];
	std::cout << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
	select(_server_fd + _client.size(), &_read_fds[1], (fd_set *)0, (fd_set *)0, (struct timeval*)0);
}

void Socket::acceptClient(void) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addrlen = sizeof(sockaddr_in);

	if ((fd = accept(_server_fd, (struct sockaddr*)&addr, (socklen_t*)&addrlen)) == -1)
		_perrorExit("accept failed");
	_client.insert(std::pair<int, sockaddr_in>(fd, addr));
	FD_SET(fd, &_read_fds[0]);
	std::cerr << "\033[1;35mAdd client " << fd << "\033[0m" << std::endl;
}

bool Socket::communicate(map::iterator it) {
	int rd;

	ioctl(it->first, FIONREAD, &rd);
	if (!rd) {
		std::cerr << "\033[1;35mRemoving client " << it->first << "\033[0m" << std::endl;
		close(it->first);
		FD_CLR(it->first, &_read_fds[0]);
		_client.erase(it->first);
	}
	return (rd);
}

std::string Socket::getHeaderRequest(int fd) const {
	char buffer[BUFFER_SIZE];
	long rd = recv(fd, buffer, BUFFER_SIZE, 0);

	if (rd == -1)
		_perrorExit("recv failed");
	buffer[rd] = '\0';
	std::cerr << "\033[1;35mServing client " << fd << "\033[0m" << std::endl;
	return (buffer);
}

Socket::map& Socket::getClient(void) {
	return (_client);
}

fd_set* Socket::getReadFds(int n) {
	return (&_read_fds[n]);
}

int Socket::getServerFd(void) const {
	return (_server_fd);
}

void Socket::_perrorExit(std::string err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
