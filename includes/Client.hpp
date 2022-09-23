#pragma once
#include "Shared.hpp"
#include "Utility.hpp"
#include "Response.hpp"

class Client 
{
	public:
	Client(int epoll, server_map::iterator& server, std::set<Client*> *clients);
	virtual ~Client();
	void disconnect(void);
	void handleEvent(uint32_t revents);
	void handle_request(void);
	int respond(void);
	int					_epoll_fd;
	server_map::iterator& _servers;
	std::set<Client*>	*_clients;

	private:
	ssize_t _receive(void);
	void _addEventListener(uint32_t revents);

	private:
	int					_fd;
	sockaddr_in			_address;
	Message				_request;
	Response			_response;
};
