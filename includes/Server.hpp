#pragma once

#include "Header.hpp"

class Server
{
	public:
	Server(t_config config);
	~Server();

	int get_server_fd(void);
	int init_socket(void);

	private:
		t_config	_config;
};
