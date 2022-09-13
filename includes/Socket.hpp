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
# include "Client.hpp"

# define BUFFER_SIZE 4096
# define MAX_EVENTS 128

class Socket
{
	public:
		Socket();
		~Socket();
		void init_epoll(void);
		void init_socket(const std::string& address, int port);
		void event_loop(void);

	private:
		// void _handle_client_event(int fd, uint32_t revents);
		// void _accept_new_client(int server);
		void _epoll_add(int fd, uint32_t revents);
		// void _close_connection(int fd);
		void _exit_error(const std::string& err) const;

	private:
		int					_epoll_fd;
		std::set<int>		_servers;
		std::set<Client*>	_clients;

};


#endif
