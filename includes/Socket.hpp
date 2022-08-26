#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
# define BUFFER_SIZE 4096
# include <string>
# include <iostream>
# include <map>
# include <cstdlib>
# include <cstdio>
# include <cerrno>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/ioctl.h>

class Socket
{
	public:
		Socket();
		~Socket();
	
		void initialize(const std::string& address, unsigned int port);
		int acceptClient(void);
		std::string getHeaderRequest(int fd) const;
		int getClientFd(void) const;
		int getServerFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	private:
		int 						_server_fd;

	public:
		struct sockaddr_in			_server_addr;
		std::map<int, sockaddr_in>	_client;
};

#endif
