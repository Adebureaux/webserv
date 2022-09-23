#pragma once
#include "Client.hpp"
#include "Conf.hpp"

class Cluster
{
	public:
	Cluster(const Conf& config);
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
	server_map_tmp			_servers_tmp;
	std::set<Client*>		_clients;
};
