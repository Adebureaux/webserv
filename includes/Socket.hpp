#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
# include <string>
# include <iostream>
# include <map>
# include <set>
# include <cstdlib>
# include <cstdio>
# include <cerrno>
# include <cstring>
# include <csignal>
# include <unistd.h>
# include <fcntl.h>
# include <sys/epoll.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <sys/ioctl.h>
# include "Request.hpp"
# include "Response.hpp"

# define BUFFER_SIZE 4096
# define PRERF "(errno=%d) %s\n"
# define PREAR(NUM) NUM, strerror(NUM)
# define EPOLL_MAP_TO_NOP (0u)
# define EPOLL_MAP_SHIFT  (1u) /* Shift to cover reserved value MAP_TO_NOP */
# define MAX_EVENTS 32
 
class Socket
{
	public:
		Socket();
		~Socket();

		void init_epoll(void);
		int init_socket(void);
		int event_loop(void);


	private:
		void _handle_client_event(int client_fd, uint32_t revents);
		void _accept_new_client(void);
		void _epoll_add(int fd, uint32_t events);
		void _close_connection(int fd);
		void _exit_error(const std::string& err) const;

	private:
		int					_server_fd;
		int					_epoll_fd;
		/*
		* Map the file descriptor to client_slot array index
		* Note: We assume there is no file descriptor greater than 10000.
		*
		* You must adjust this in production.
		*/
		// uint32_t			_client_map[10000];
		std::map<int, sockaddr_in>				_client_slot;

};


#endif
