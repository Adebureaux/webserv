#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket(void)
{
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("cannot create socket");
		exit(errno);
	}
	int enable = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		perror("cannot set socket option 'SO_REUSEADDR'");
	identify();
	wait();
	communicate();
}

SimpleSocket::~SimpleSocket(void)
{
	close(_socket);
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
	{
		perror("bind failed"); 
		exit(errno); 
	}
}

void SimpleSocket::wait(void)
{
	/* Change the '3' value after Charles parsing */
	if (listen(_server_fd, 3) == -1)
	{
		perror("In listen");
		exit(errno);
	}
	unsigned int addrlen = sizeof(_address);
	if ((_socket = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen)) == -1)
	{
		perror("In accept");
		exit(errno);
	}
}

void SimpleSocket::communicate(void)
{
	char buffer[1024] = {0};
	int valread = read(_socket, buffer, 1024);
	std::cout << buffer;
	if (valread == -1)
			exit(1);
	std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	write(_socket, hello.c_str(), hello.size());
}
