#include "Client.hpp"
#include <sys/stat.h>

#include <sys/types.h>

#include <sys/sysmacros.h>


Client::Client(int epoll, int server_fd, config_map *config, std::set<Client*> *clients) : _epoll_fd(epoll), _config(config), _clients(clients), _request(this)
{
	socklen_t addr_len = sizeof(_address);

	std::memset(&_address, 0, addr_len);
	if ((_fd = accept(server_fd, (sockaddr*)&_address, &addr_len)) < 0)
		throw std::exception();
	if (DEBUG)
		std::cout << C_G_MAGENTA << "Add client " << _fd << " on server " << server_fd << C_RES <<std::endl;
	_addEventListener(EPOLLOUT | EPOLLIN | EPOLLERR | EPOLLRDHUP);
};

Client::~Client()
{
	if (DEBUG)
		std::cout << C_G_MAGENTA << "Destruct client " << _fd << C_RES << std::endl;
	disconnect();
	_clients->erase(this);
};

void Client::_addEventListener(uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.ptr = this;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _fd, &event) < 0)
		throw std::exception();
};

void Client::disconnect(void)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _fd, NULL);
	close(_fd);
};

ssize_t Client::_receive(void)
{
	char buffer[BUFFER_SIZE + 1] = { 0 };
	int ret = recv(_fd, buffer, BUFFER_SIZE, 0);
	if (ret > 0)
	{
		_request.raw_data.append(buffer, ret);
		_request.current_content_size += ret;
		if (_request.header_end != std::string::npos or (_request.header_end = _request.raw_data.find(__DOUBLE_CRLF, 0 , 4)) != std::string::npos)
		{
			_request.state = READY;
		}
		else _request.state = INCOMPLETE;
	}
	return (ret);
};

static void setPostOptions(Message &req)
{
	req.post_options_set = true;
	std::pair<bool, std::string> res = req.info.get_header_var_by_name("Content-Length");
	if (res.first)
	{
		req.indicated_content_size = std::atoi(res.second.c_str());
		req.header_size = req.header_end + 4;
		req.current_content_size -= req.header_size;
		res = req.info.get_header_var_by_name("Content-Type");
		if (res.first and res.second.find("multipart/") != std::string::npos)
		{
			size_t pos = res.second.find("; boundary=");
			if (pos != std::string::npos)
			{
				req.multipart = true;
				req.boundary = res.second.substr(pos + 11, res.second.size());
				req.boundary.insert(0, "--");
				req.boundary_end = req.boundary;
				req.boundary_end.append("--");
			}
			else
			{
				req.info._is_valid = false;
				req.response_override = 400;
				return ;
			}
		}
		if ((res = req.info.get_header_var_by_name("Expect")).first and res.second == "100-continue")
		{
			req.continue_100 = READY;
			req.response_override = 100;
		}
		if (req.continue_100 and !req.multipart)
		{
			req.info._is_valid = false;
			req.response_override = 400;
			return ;
		}
	}
	else req.response_override = 411;
};

void Client::handleEvent(uint32_t revents)
{
	if (revents & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
		throw std::exception();
	if (revents & EPOLLIN)
	{
		long long ret = 0;
		if ((ret = _receive()) < 0)
			throw std::exception();
		else if (ret == 0)
			;
		else handle_request();
	}
	if (revents & EPOLLOUT and (_request.state == READY or (_request.state == INCOMPLETE and _request.continue_100 == READY)))
	{

		respond();
		if (_request.continue_100 == READY)
			_request.continue_100 = DONE;
		else if (_request.state != INCOMPLETE)
		{
			if (_request.info.get_connection() != "keep-alive")
				throw std::exception();
			_request.reset();
		}
	}
};

void Client::handle_request(void)
{
	if (_request.state == READY)
	{
		if (!_request.header_parsed)
		{
			_request.info = Request(_request.raw_data);
			_request.header_parsed = true;
		}
		if (_request.info.is_valid() and _request.info.get_method() == POST)
		{
			if (!_request.post_options_set)
				setPostOptions(_request);
			if (_request.info.is_valid() and _request.current_content_size < _request.indicated_content_size)
				_request.state = INCOMPLETE;
		}
	}
	if (_request.indicated_content_size and _request.current_content_size >= _request.indicated_content_size)
		_request.state = READY;
};

int Client::respond(void)
{
	_response.create(_request, *_config);
	send(_fd, _response.send(), _response.get_size(), 0);
	_response.clear();
	return (0);
};
