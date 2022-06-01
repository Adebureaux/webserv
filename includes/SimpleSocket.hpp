#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>

class SimpleSocket
{
	protected:
		SimpleSocket();
		~SimpleSocket();

	protected:
		int _socket;
};
#endif
