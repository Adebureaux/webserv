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
		typedef std::map<int, sockaddr_in> map;

	public:
		Socket();
		~Socket();
	
		void initialize(const std::string& address, unsigned int port);
		void waitRequest(void);
		void acceptClient(void);
		bool communicate(map::iterator it);
		std::string getHeaderRequest(int fd) const;
		map& getClient(void);
		fd_set* getReadFds(int n);
		int getServerFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	private:
		int 						_server_fd;
		struct sockaddr_in			_server_addr;
		map							_client;
		fd_set						_read_fds[2];
};

#endif
