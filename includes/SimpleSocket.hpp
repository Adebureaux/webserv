#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_EVENTS 5
#define READ_SIZE 10

class SimpleSocket
{
	public:
		SimpleSocket();
		~SimpleSocket();

		void identify(void);
		void probe(void);
		void communicate(void);
		void perror_exit(std::string err);

	private:
		int 				_server_fd;
		int					_socket_fd;
		int					_epoll_fd;
		int					_port;
		struct sockaddr_in	_address;
		struct epoll_event	_event;
		struct epoll_event	_events[MAX_EVENTS];
};
#endif
