#include "Client.hpp"

Client::Client(int epoll, int fd, server_map::iterator& servers) : _epoll_fd(epoll), _fd(fd), _servers(servers)
{
	_addEventListener(EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
};

Client::Client(const Client& rhs) : _servers(rhs._servers)
{
	*this = rhs;
};

Client& Client::operator=(const Client& rhs)
{
	_epoll_fd = rhs._epoll_fd;
	_fd = rhs._fd;
	_servers = rhs._servers;
	_request = rhs._request;
	_response = rhs._response;
	return *this;
};

Client::~Client()
{
	std::cout << C_G_RED << "ICI DESTRUCTEUR" << C_RES << std::endl;
	disconnect();
	//remove all messages
};

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

void Client::_addEventListener(uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.fd = _fd;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _fd, &event) < 0)
		exit(-2);
	std::cout << "\tadded to epoll loop"<< std::endl;

}

void Client::disconnect(void)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _fd, NULL);
	close(_fd);
};

void Client::handleEvent(uint32_t revents)
{
	if (revents & (EPOLLERR | EPOLLHUP))
	{
		disconnect();// must close collection and destroy client as well as its ref in the clients collection
		throw std::exception();
		return;
	}
	if (revents & EPOLLIN)
	{
		if (_receive() < 0)
		{
			disconnect(); // must close connection and destroy client
			throw std::exception();
			return;
		}
	}
	if (revents & EPOLLOUT && _request.state == READY)
	{
		handle_request();
		respond();
		_request.raw_data.erase();
		_request.state = INCOMPLETE;
	}
};

void Client::handle_request(void)
{
	_request.info = Request(_request.raw_data);
	//std::cout << "Received request semantics : " << (request.info.is_valid() ? "valid\n" : "invalid\n");
};

int Client::respond(void)
{
	// We should find here which config of server we pass to Response, instead of the hard coded "webserv.fr"
	// If host does not exist, that's the first server for this host:port who should serve the client
	_response.create(_request.info, _servers->second.find("webserv.fr"));
	send(_fd, _response.send(), _response.get_size(), 0);
	_response.erase();
	if (_request.info.get_connection() != "keep-alive") // Leak with keep-alive !
	{
		disconnect();
		throw std::exception();
	}
	return (0);
};
