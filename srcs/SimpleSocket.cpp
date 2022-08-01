#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket() {}

const char *SimpleSocket::create_socket(unsigned int port)
{
	_port = port;

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);

	// Socket domain and type
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return ("cannot create socket");

	// Socket option to reuse our address
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		return ("cannot set socket option 'SO_REUSEADDR'");

	return (NULL);
}

const char *SimpleSocket::identifySocket() 
{
	if (sizeof(this->_address) < 0)
        return ("bind failed");

	if (bind(_server_fd, (struct sockaddr *)&this->_address, sizeof(this->_address)) != 0) 
		return ("bind failed");
	return (NULL); 
}

const char *SimpleSocket::listenSocket(void) const 
{
	if (listen(_server_fd, 1) != 0)
		return ("listen failed");
	return (NULL);
}

int SimpleSocket::acceptSocket(void) 
{
	int addrlen = sizeof(_address);

	if (addrlen < 0)
		return (-1);

	return accept(this->_server_fd, (struct sockaddr *)&this->_address, (socklen_t*)&addrlen);
}

int SimpleSocket::getServerFd(void) const 
{
	return this->_server_fd;
}

void SimpleSocket::_perrorExit(std::string err) const 
{
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~SimpleSocket();
	exit(errno);
}
