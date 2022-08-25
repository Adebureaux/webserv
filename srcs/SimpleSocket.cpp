#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket() {
	int opt = 1;

	// Socket domain and type
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		_perrorExit("cannot create socket");

	// Socket option to reuse our address
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		_perrorExit("cannot set socket option 'SO_REUSEADDR'");
}

SimpleSocket::~SimpleSocket(void) {
	close(_server_fd);
	close(_socket_fd);
}

void SimpleSocket::identifySocket(unsigned int port) {
	_port = port;
	memset((char*)&_server_address, 0, sizeof(_server_address));
	_server_address.sin_family = AF_INET;
	_server_address.sin_addr.s_addr = htonl(INADDR_ANY); // IP HERE
	_server_address.sin_port = htons(_port); // PORT HERE
	if (bind(_server_fd, (struct sockaddr*)&_server_address, sizeof(_server_address)) == -1) 
		_perrorExit("bind failed"); 
}

void SimpleSocket::listenSocket(void) const {
	if (listen(_server_fd, 5) == -1)
		_perrorExit("listen failed");
}

void SimpleSocket::acceptSocket(void) {
	unsigned int addrlen = sizeof(_client_address);

	if ((_socket_fd = accept(_server_fd, (struct sockaddr*)&_client_address, (socklen_t*)&addrlen)) == -1)
		_perrorExit("accept failed");
}

std::string SimpleSocket::communicateSocket(int fd) const {
	char buffer[BUFFER_SIZE];
	int rd = read(fd, buffer, BUFFER_SIZE);

	if (rd == -1)
		_perrorExit("read failed");
	buffer[rd] = '\0';
	return (buffer);
}

int SimpleSocket::getSocketFd(void) const {
	return (_socket_fd);
}

int SimpleSocket::getServerFd(void) const {
	return (_server_fd);
}

void SimpleSocket::_perrorExit(std::string err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~SimpleSocket();
	exit(errno);
}
