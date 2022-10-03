#include "Client.hpp"

Client::Client(int epoll, int server_fd, config_map *config, std::set<Client*> *clients) : _epoll_fd(epoll), _config(config), _clients(clients), _request(this)
{
	socklen_t addr_len = sizeof(_address);

	std::memset(&_address, 0, addr_len);
	if ((_fd = accept(server_fd, (sockaddr*)&_address, &addr_len)) < 0)
	{
		throw std::exception();
	}
	if (DEBUG)
		std::cout << C_G_MAGENTA << "Add client " << _fd << " on server " << server_fd << C_RES <<std::endl;
	_addEventListener(EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
};

Client::~Client()
{
	if (DEBUG)
		std::cout << C_G_MAGENTA << "Destruct client " << _fd << C_RES << std::endl;
	disconnect();
	_clients->erase(this);

	//remove all messages
};

ssize_t Client::_receive(void)
{
	ssize_t received = 0;

	for(;;)
	{
		char buffer[BUFFER_SIZE + 1] = { 0 };
		int ret = recv(_fd, buffer, BUFFER_SIZE, 0);
		if (ret == 0 || ret < 0)
			break;
		if (ret > 0)
		{
			std::cout << ret << std::endl;

			_request.raw_data.append(buffer);
			received += ret;
		}
		if (ret < BUFFER_SIZE)
			break;
	}
	// std::cout << "COUCOU\n";
	if (_request.raw_data.find("\r\n\r\n") != std::string::npos)
		_request.state = READY;
	// static char buffer[BUFFER_SIZE];
	// ssize_t rd;
	//
	// std::memset(buffer, 0, BUFFER_SIZE);
	// rd = recv(_fd, buffer, BUFFER_SIZE, 0);
	// _request.raw_data.append(buffer);
	return (received);
}

void Client::_addEventListener(uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.ptr = this;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _fd, &event) < 0)
		throw std::exception();

}

void Client::disconnect(void)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _fd, NULL);
	close(_fd);
};

void Client::handleEvent(uint32_t revents)
{
	if (revents & (EPOLLERR | EPOLLHUP))
		throw std::exception();
	if (revents & EPOLLIN)
	{
		if (_receive() <= 0)
			throw std::exception();
	}
	if (revents & EPOLLOUT && _request.state == READY)
	{
		handle_request();
		respond();
		_request.raw_data.clear();
		_request.state = INCOMPLETE;
	}
};

void Client::handle_request(void)
{
	if (DEBUG)
	{
		std::cout << C_G_YELLOW << "---------- REQUEST ----------" << std::endl;
		std::cout << _request.raw_data << std::endl;
		std::cout << "-----------------------------" << C_RES << std::endl << std::endl;
	}
	_request.info = Request(_request.raw_data);
};

int Client::respond(void)
{
	_response.create(_request.info, *_config);
	send(_fd, _response.send(), _response.get_size(), 0);
	if (DEBUG)
	{
		std::cout << C_G_GREEN << "---------- RESPONSE ---------" << std::endl;
		std::cout << (const char*)_response.send() << std::endl;
		std::cout << "-----------------------------" << C_RES << std::endl << std::endl;
	}
	_response.clear();
	if (_request.info.get_connection() != "keep-alive")
		throw std::exception();
	return (0);
};
