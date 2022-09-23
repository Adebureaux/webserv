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
	void _add_client(server_map::iterator& servers);
	int _init_socket(t_server_block config);

	private:
	int						_epoll_fd;
	int						_server_number;
	server_map				_servers; // int -> server_fd / string -> server_name / t_server_block -> config
	std::map<int, Client>	_clients;
};
