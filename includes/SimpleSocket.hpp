#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

class SimpleSocket
{
	public:
		SimpleSocket(void);
		~SimpleSocket(void);

		void identify(void);
		void wait(void);
		void communicate(void);

	private:
		int 				_server_fd;
		int					_port;
		int					_socket;
		struct sockaddr_in _address;
};
#endif
