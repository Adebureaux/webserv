#include "Socket.hpp"

// https://bousk.developpez.com/cours/reseau-c++/TCP/05-envoi-reception-serveur/#LIII-B

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
	if (listen(_server_fd, 100) == -1) // Number of client listned
		_perrorExit("listen failed");
}

int Socket::acceptClient(void) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addrlen = sizeof(sockaddr_in);

	if ((fd = accept(_server_fd, (struct sockaddr*)&addr, (socklen_t*)&addrlen)) == -1)
		_perrorExit("accept failed");
	std::cerr << "\033[1;35mAdd client " << fd << "\033[0m" << std::endl;
	_client.insert(std::pair<int, sockaddr_in>(fd, addr));
	return (fd);
}

std::string Socket::getHeaderRequest(int fd) const {
	char buffer[BUFFER_SIZE];
	long rd = recv(fd, buffer, BUFFER_SIZE, 0);

	if (rd == -1)
		_perrorExit("recv failed");
	buffer[rd] = '\0';
	return (buffer);
}

int Socket::getClientFd(void) const {
	return (_client.rbegin()->first);
}

int Socket::getServerFd(void) const {
	return (_server_fd);
}

void Socket::_perrorExit(std::string err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
