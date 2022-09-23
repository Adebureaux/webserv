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
	void _handle_event(Client& client, uint32_t revents);
	void _accept_new_client(server_map::iterator& server);
	int _init_socket(t_server_block config);
	void _epoll_add(int fd, uint32_t revents);
	void _disconnect(int fd);

	private:
	int						_epoll_fd;
	int						_server_number;
	server_map				_servers; // int -> server_fd / string -> server_name / t_server_block -> config
	std::set<Client*>		_clients;
	std::map<int, Client>	_client_map;
};
