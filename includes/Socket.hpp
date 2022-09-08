#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
# include <string>
# include <iostream>
# include <set>
# include <cstdlib>
# include <cerrno>
# include <cstring>
# include <csignal>
# include <unistd.h>
# include <sys/epoll.h>
# include <arpa/inet.h>
# include "Request.hpp"
# include "Response.hpp"

# define BUFFER_SIZE 4096
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
		int							_server_fd;
		int							_epoll_fd;
		std::set<int>				_client_slot;

};


#endif
