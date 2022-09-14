#pragma once
#include "Header.hpp"
#include "Server.hpp"
#include "Client.hpp"

class Cluster
{
	public:
	Cluster();
	~Cluster();

	void parse(const std::string& file);
	void event_loop(void);

	private:
	void _epoll_add(int fd, uint32_t revents);

	private:
	int						_epoll_fd;
	int						_server_number;
	std::map<int, Server>	_servers;
	std::set<Client*>		_clients;
};
