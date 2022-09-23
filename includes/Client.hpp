#pragma once
#include "Shared.hpp"
#include "Utility.hpp"
#include "Response.hpp"

typedef std::map<std::string, Server_block> config_map;
typedef std::map<int, config_map> server_map;

class Client
{
	public:
	Client(int epoll, int server_fd, config_map& config, std::set<Client*> *clients);
	virtual ~Client();
	void disconnect(void);
	void handleEvent(uint32_t revents);
	void handle_request(void);
	int respond(void);

	private:
	ssize_t _receive(void);
	void _addEventListener(uint32_t revents);

	private:
	int						_fd;
	int						_epoll_fd;
	config_map&				_config;
	std::set<Client*>		*_clients;
	sockaddr_in				_address;
	Message					_request;
	Response				_response;
};
