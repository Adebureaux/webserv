#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
# define BUFFER_SIZE 4096
# include <string>
# include <iostream>
# include <set>
# include <cstdlib>
# include <cstdio>
# include <cerrno>
# include <cstring>
# include <csignal>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/ioctl.h>
# include "Request.hpp"
# include "Response.hpp"

class Socket
{
	public:
		Socket();
		~Socket();
	
		void initialize(void);
		void waitRequest(void);
		void acceptClient(void);
		void communicate(void);

	private:
		void _perrorExit(const std::string& err) const;

	private:
		int 				_server_fd;
		struct sockaddr_in	_server_addr;
		fd_set				_readfds;
		fd_set				_writefds;
};


#endif
