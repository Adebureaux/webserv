#pragma once
#include "Client.hpp"

class Cluster
{
	public:
	Cluster();
	~Cluster();

	void parse(const std::string& file);
	void event_loop(void);

	private:
	void _add_server(int fd, uint32_t revents);
	int _init_socket(t_server_block config);

	private:
	int						_epoll_fd;
	int						_server_number;
	server_map				_servers;
	std::set<Client*>		_clients;
};
