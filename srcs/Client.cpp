#include "Client.hpp"


Client::Client(int fd, server_map::iterator& servers) : _fd(fd), _servers(servers), _request(this) {};

Client::~Client() {};

ssize_t Client::_receive(void)
{
	static char buffer[BUFFER_SIZE];
	ssize_t rd;

	std::memset(buffer, 0, BUFFER_SIZE);
	rd = recv(_fd, buffer, BUFFER_SIZE, 0);

	if (rd > 0)
	{
		_request.raw_data.append(buffer);
		if (_request.raw_data.find("\r\n\r\n") != std::string::npos)
			_request.state = READY;
	}
	return (rd);
}

void Client::handle_request(void)
{
	_request.info = Request(_request.raw_data);
};


int Client::respond(void)
{
	// We should find here which config of server we pass to Response, instead of the hard coded "webserv.fr"
	// If host does not exist, that's the first server for this host:port who should serve the client
	_response.create(_request.info, _servers->second.find("webserv.fr"));
	send(_fd, _response.send(), _response.get_size(), 0);
	_response.erase();
	if (_request.info.get_connection() != "keep-alive") // Leak with keep-alive !
		return (0);
	_request.raw_data.erase();
	_request.state = INCOMPLETE;
	return (1);
};

int Client::get_request_state(void)
{
	return (_request.state);
}

int Client::get_fd(void)
{
	return (_fd);
}
