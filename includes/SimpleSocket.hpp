#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
# define BUFFER_SIZE 4096
# include <string>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <cerrno>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/ioctl.h>

class SimpleSocket
{
	public:
		SimpleSocket();
		~SimpleSocket();
	
		void identifySocket(unsigned int port);
		void listenSocket(void) const;
		void acceptSocket(void);
		std::string communicateSocket(int fd) const;
		int getSocketFd(void) const;
		int getServerFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	private:
		int 				_server_fd;
		int					_socket_fd;
		int					_port;
		struct sockaddr_in	_server_address;
		struct sockaddr_in	_client_address;
};

#endif
