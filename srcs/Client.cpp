#include "Client.hpp"

Client::Client(int epoll, server_map::iterator& server, std::set<Client *> *clients) : _epoll_fd(epoll), _server(server), _clients(clients), _request(this)
{
	socklen_t addr_len = sizeof(_address);

	std::memset(&_address, 0, addr_len);
	if ((_fd = accept(server->first, (sockaddr*)&_address, &addr_len)) < 0)
	{
		if (errno == EAGAIN)
			return; // should thow an exception to force auto deletion because of new construction
		exit(-1);  // should thow an exception to force auto deletion because of new construction
	}
	std::cout << "new client with fd: " << _fd << " accepted on server:" << server->first << std::endl;
	_addEventListener(EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
};

Client::~Client()
{
	(void)disconnect();
	_clients->erase(this);
	//remove all messages
};

int Client::_receive(void)
{
	static char buffer[BUFFER_SIZE];
	std::stringstream res;
	int ret = 0;
	if ((ret = recv(_fd, buffer, sizeof(buffer), 0)) > 0)
	{
		res << buffer;
		std::memset(buffer, 0, ret);
	}
	else if (ret <= 0)
		disconnect();
	_request.raw_data.append(res.str());
	if (_request.raw_data.find("\r\n\r\n") != std::string::npos)
	{
		_request.state = READY;
	}
	if (ret < 0)
		return ret;
	// std::cout << "received request:\n" << request.raw_data << std::endl;
	return (res.str().size());
}

void Client::_addEventListener(uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.ptr = this;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _fd, &event) < 0)
		exit(-2);
	std::cout << "\tadded to epoll loop"<< std::endl;

}

void Client::disconnect(void)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _fd, NULL);
	close(_fd);
	_clients->erase(this);
};

void Client::handleEvent(uint32_t revents)
{
	int recv_ret = 0;

	if (revents & (EPOLLERR | EPOLLHUP))
	{
		disconnect();// must close collection and destroy client as well as its ref in the clients collection
		throw std::exception();
		return;
	}
	if (revents & EPOLLIN)
	{
		recv_ret = _receive();
		if (recv_ret <= 0) {
			disconnect(); // must close collection and destroy client
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
	std::cout << _request.raw_data;
	_request.info = Request(_request.raw_data);
	//std::cout << "Received request semantics : " << (request.info.is_valid() ? "valid\n" : "invalid\n");
};

int Client::respond(void)
{
	// We should find here which config of server we pass to Response, instead of the hard coded "webserv.fr"
	// If host does not exist, that's the first server for this host:port who should serve the client
	_response.create(_request.info, _server->second.find("weebserv.fr"));
	send(_fd, _response.send(), _response.get_size(), 0);
	_response.erase();
	if (_request.info.get_connection() != "keep-alive") // Leak with keep-alive !
	{
		disconnect();
		throw std::exception();
	}
	return (0);
};
