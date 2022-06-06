#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket(void)
{
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror_exit("cannot create socket");
	int enable = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		perror_exit("cannot set socket option 'SO_REUSEADDR'");
	identify();
	while (1)
	{
		probe();
		communicate();
	}
}

SimpleSocket::~SimpleSocket(void)
{
	close(_server_fd);
	close(_socket_fd);
	close(_epoll_fd);
}

void SimpleSocket::identify(void)
{
	_port = 8080; // Where the clients can reach at
	/* htonl converts a long integer (e.g. address) to a network representation */ 
	/* htons converts a short integer (e.g. port) to a network representation */ 
	memset((char*)&_address, 0, sizeof(_address)); 
	_address.sin_family = AF_INET; 
	_address.sin_addr.s_addr = htonl(INADDR_ANY); 
	_address.sin_port = htons(_port);
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1) 
		perror_exit("bind failed"); 
}

void SimpleSocket::probe(void)
{
	if (listen(_server_fd, 1) == -1)
		perror_exit("listen failed");
	unsigned int addrlen = sizeof(_address);
	if ((_socket_fd = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen)) == -1)
		perror_exit("accept failed");
}

void SimpleSocket::communicate(void)
{
	char buffer[1024] = {0};
	int valread = read(_socket_fd, buffer, 1024);
	if (valread == -1)
		exit(1);
	std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	std::string hell2 = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nBjrlemonde!!";
	write(_socket_fd, hello.c_str(), hello.size());
	write(_socket_fd, hell2.c_str(), hell2.size());
}

void SimpleSocket::perror_exit(std::string err)
{
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~SimpleSocket();
	exit(errno);
}
