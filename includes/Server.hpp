#pragma once

#include "Header.hpp"

class Server
{
	public:
	Server(t_server_block config);
	~Server();

	int init_socket(void);

	public:
	int				server_fd;
	t_server_block	config;
};
