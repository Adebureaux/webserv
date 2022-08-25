#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
# define BUFFER_SIZE 4096
# include <string>
# include <iostream>
# include <vector>
# include <cstdlib>
# include <cstdio>
# include <cerrno>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sys/ioctl.h>

struct Client {
	int					fd;
	struct sockaddr_in	addr;
	unsigned int		addrlen;
};

class Socket
{
	public:
		Socket();
		~Socket();
	
		void initialize(const std::string& address, unsigned int port);
		void acceptClient(void);
		std::string getHeaderRequest(void) const;
		int selects(void) const;
		int getClientFd(void) const;
		int getServerFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	private:
		int 				_server_fd;
		struct sockaddr_in	_server_addr;
		std::vector<Client>	_client;
};

#endif
