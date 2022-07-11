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
		~SimpleSocket();
	
		void identifySocket(unsigned int port);
		void listenSocket(void) const;
		void acceptSocket(void);
		void communicateSocket(void) const;
		void HTTPGet(const char* filename) const;
		int getSocketFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	private:
		int 				_server_fd;
		int					_socket_fd;
		int					_port;
		struct sockaddr_in	_address;
};
#endif
