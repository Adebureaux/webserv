#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_EVENTS 5
#define READ_SIZE 10
#define SSTR(x) static_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

class SimpleSocket
{
	public:
		SimpleSocket();
	
		const char *create_socket(unsigned int port);
		const char *identifySocket();
		const char *listenSocket(void) const;
		int acceptSocket(void);
		int getServerFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	public:
		int 				_server_fd;
		int					_port;
		struct sockaddr_in	_address;
};

/*
struct epoll_event {
               uint32_t     events;    // Epoll events
               epoll_data_t data;      // User data variable
           };*/

#endif
