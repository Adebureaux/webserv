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
	fd_set set;
	close(_server_fd);
	for (std::vector<Client>::iterator it = _client.begin(); it < _client.end(); it++) {
		FD_CLR(it->fd, &set);
		close(it->fd);
	}
}

void Socket::initialize(const std::string& address, unsigned int port) {
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	inet_aton(address.c_str(), &_server_addr.sin_addr);
	// _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// fcntl(_server_fd, F_SETFL, O_NONBLOCK); // SETUP SELECT FIRST
	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) 
		_perrorExit("bind failed"); 
	if (listen(_server_fd, 5) == -1) // Number of client listned
		_perrorExit("listen failed");
}

void Socket::acceptClient(void) {
	Client client;

	client.addrlen = sizeof(sockaddr_in);
	if ((client.fd = accept(_server_fd, (struct sockaddr*)&client.addr, (socklen_t*)&client.addrlen)) == -1)
		_perrorExit("accept failed");
	_client.push_back(client);
}

std::string Socket::getHeaderRequest(void) const {
	char buffer[BUFFER_SIZE];
	long rd = read(getClientFd(), buffer, BUFFER_SIZE);

	if (rd == -1)
		_perrorExit("read failed");
	buffer[rd] = '\0';
	return (buffer);
}

int Socket::selects(void) const {
	fd_set setReads;
	fd_set setWrite;
	fd_set setErrors;
	int highestFd = 0;
	for (std::vector<Client>::const_iterator it = _client.begin(); it < _client.end(); it++) {
		FD_SET(it->fd, &setReads);
		FD_SET(it->fd, &setWrite);
		FD_SET(it->fd, &setErrors);
		if (it->fd > highestFd)
			highestFd = it->fd;
	}
	return (select(highestFd + 1, &setReads, &setWrite, &setErrors, NULL));
}

int Socket::getClientFd(void) const {
	return (_client[_client.size() - 1].fd);
}

int Socket::getServerFd(void) const {
	return (_server_fd);
}

void Socket::_perrorExit(std::string err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
