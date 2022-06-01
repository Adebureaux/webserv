#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class SimpleSocket
{
	public:
		SimpleSocket();
		~SimpleSocket();

	private:
		int _socket;
		int _csocket;
};
#endif
