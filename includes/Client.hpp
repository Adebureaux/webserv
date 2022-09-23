#pragma once
#include "Shared.hpp"
#include "Utility.hpp"
#include "Response.hpp"

class Client 
{
	public:
	Client(int epoll, int fd, server_map::iterator& server);
	Client(const Client& rhs);
	Client& operator=(const Client& rhs);
	virtual ~Client();
	void disconnect(void);
	void handleEvent(uint32_t revents);
	void handle_request(void);
	int respond(void);

	private:
	ssize_t _receive(void);
	void _addEventListener(uint32_t revents);

	private:
	int						_epoll_fd;
	int						_fd;
	server_map::iterator& 	_servers;
	Message					_request;
	Response				_response;
};
