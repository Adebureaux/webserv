#pragma once
#include "Client.hpp"
#include "Conf.hpp"

class Cluster
{
	public:
	Cluster(server_map& config);
	~Cluster();

	void parse(const std::string& file);
	void event_loop(void);

	private:
	void _add_server(int fd, uint32_t revents) const;
	int _init_socket(const Server_block& config) const;

	private:
	int							_epoll_fd;
	server_map					_servers;
	std::set<Client*>			_clients;
};
