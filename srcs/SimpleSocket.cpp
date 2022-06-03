#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket(void)
{
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror_exit("cannot create socket");
	int enable = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		perror_exit("cannot set socket option 'SO_REUSEADDR'");
	identify();
	probe();
	communicate();
}

SimpleSocket::~SimpleSocket(void)
{
	close(_server_fd);
	close(_socket_fd);
	// close(_epoll_fd);
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
	/* epoll work in progress */

	// if ((_epoll_fd = epoll_create1(0)) == -1)
	// 	perror_exit("epoll_create1 failed");
	// _event.events = EPOLLIN;
	// _event.data.fd = 0;
	// if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &_event))
	// 	perror_exit("adding epoll file descriptor failed");
	// int running = 1, event_count, i;
	// size_t bytes_read;
	// while (running) {
	// 	printf("\nPolling for input...\n");
	// 	event_count = epoll_wait(_epoll_fd, _events, MAX_EVENTS, 30000);
	// 	printf("%d ready events\n", event_count);
	// 	for (i = 0; i < event_count; i++) {
	// 		printf("Reading file descriptor '%d' -- ", _events[i].data.fd);
	// 		bytes_read = read(_events[i].data.fd, read_buffer, READ_SIZE);
	// 		printf("%zd bytes read.\n", bytes_read);
	// 		read_buffer[bytes_read] = '\0';
	// 		printf("Read '%s'\n", read_buffer);
	// 		if(!strncmp(read_buffer, "stop\n", 5))
	// 			running = 0;
	// 	}
	// }

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
	std::string hello = "HTTP/1.1 200 OK";
	write(_socket_fd, hello.c_str(), hello.size());
}

void SimpleSocket::perror_exit(std::string err)
{
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~SimpleSocket();
	exit(errno);
}
