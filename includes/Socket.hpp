#ifndef SIMPLE_SOCKET_HPP
# define SIMPLE_SOCKET_HPP
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
 
struct client_slot {
	bool				is_used;
	int				 	client_fd;
	char				src_ip[sizeof("xxx.xxx.xxx.xxx")];
	uint16_t			src_port;
	uint16_t			my_index;
};

class Socket
{
	public:
		Socket();
		~Socket();

		void init_state(void);
		void init_epoll(void);
		int init_socket(void);
		int event_loop(void);


	private:
		void _handle_client_event(int client_fd, uint32_t revents);
		int _accept_new_client(int tcp_fd);
		const char* _convert_addr_ntop(struct sockaddr_in *addr, char *src_ip_buf);
		int _my_epoll_delete(int epoll_fd, int fd);
		int _my_epoll_add(int epoll_fd, int fd, uint32_t events);
		void _exit_error(const std::string& err) const;

	private:
		int					_server_fd;
		int					_epoll_fd;
		struct client_slot	_clients[10];
		/*
		* Map the file descriptor to client_slot array index
		* Note: We assume there is no file descriptor greater than 10000.
		*
		* You must adjust this in production.
		*/
		uint32_t			_client_map[10000];

};


#endif
