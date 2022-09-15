#pragma once
#include "Request.hpp"
#include "Server.hpp"

typedef enum e_state {
	ERROR = -1 , WAITING, INCOMPLETE, READY, DONE
} t_state;

class Client;

class Message
{
public:
	Client			*client;
	t_state			state;
	std::string		raw_data;
	// std::string		header;
	// std::string		body;
	Message			*ptr; // response || request
	Request			info;

	Message(Client *c);
	virtual ~Message();
	const char *data(void) const;// should return complete response buffer; wether it's an error or correct page
	size_t size(void) const; // should return response buffer size
};

class Client {

private:
	int _receive(void);
	void _addEventListener(uint32_t revents);
public:
	int					fd;
	int					epoll_fd;
	Server&				_server; // should take instead a pointer to a server
	std::set<Client*>	*_clients;
	sockaddr_in			address;
	Message				request;
	Message				response;

	Client(int epoll, Server& server, std::set<Client *> *clients);
	virtual ~Client();
	void disconnect(void);
	void handleEvent(uint32_t revents);
	void handle_request(void);
	int respond();
};
