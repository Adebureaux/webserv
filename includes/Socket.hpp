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
		bool communicate(int fd);
		std::string getHeaderRequest(int fd) const;
		bool isReadSet(int fd) const;
		bool isWriteSet(int fd) const;
		map& getClient(void);
		int getServerFd(void) const;

	private:
		void _perrorExit(std::string err) const;

	private:
		int 				_server_fd;
		struct sockaddr_in	_server_addr;
		map					_client;
		fd_set				_master_fds;
		fd_set				_read_fds;
		fd_set				_write_fds;
};

#endif
