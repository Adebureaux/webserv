#pragma once
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Client 
{
	private:
	int _receive(void);
	void _addEventListener(uint32_t revents);

	public:
	int					fd;
	int					epoll_fd;
	Server&				_server;
	std::set<Client*>	*_clients;
	sockaddr_in			address;
	Message				request;
	Response			response;

	Client(int epoll, Server& server, std::set<Client *> *clients);
	virtual ~Client();
	void disconnect(void);
	void handleEvent(uint32_t revents);
	void handle_request(void);
	int respond();
};
