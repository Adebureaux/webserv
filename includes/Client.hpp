#pragma once
#include "Shared.hpp"
#include "Utility.hpp"
#include "Response.hpp"

class Client 
{
	public:
	Client(int fd, server_map::iterator& servers);
	~Client();

	void handle_request(void);
	int respond(void);
	int get_fd(void);
	int get_request_state(void);

	ssize_t _receive(void);

	private:
	int					_fd;
	server_map::iterator& _servers;
	Message				_request;
	Response			_response;
};
