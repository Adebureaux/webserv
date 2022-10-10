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
	_addEventListener(EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
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
	// struct stat infos;
	// fstat(_fd, &infos);
	// std::cout << C_G_CYAN << "stat on client fd: "<< infos.st_blksize << C_RES << std::endl;
	char buffer[BUFFER_SIZE + 1] = { 0 };
	int ret = recv(_fd, buffer, BUFFER_SIZE, 0);
	if (ret > 0)
		_request.raw_data.append(buffer);
	std::cerr <<_request.raw_data;

	if (_request.header_end || (_request.header_end = _request.raw_data.find(__DOUBLE_CRLF)) != std::string::npos)
		_request.state = READY;
	return (ret);
};

static void setPostOptions(Message &req)
{
	// std::cout << C_G_BLUE << "setPostOptions()\n";
	req.post_options_set = true;
	std::pair<bool, std::string> res = req.info.get_header_var_by_name("Content-Length");
	if (res.first)
	{
		req.indicated_content_size = std::atoi(res.second.c_str()); // must check later if this value doesnt exceed max_body_size (conf)

		// should also check if second is a valid number
		req.header_size = req.header_end + 5; // (header + double clrf)
		if (req.header_size < req.raw_data.size())
			req.current_content_size = req.raw_data.size() - req.header_size + 1;
		std::cout << C_G_BLUE << "setPostOptions() current content size: " << req.current_content_size << std::endl;
		res = req.info.get_header_var_by_name("Content-Type");

		// MULTIPART REQUEST
		// if (res.first && res.second.find("/x-www-form-urlencoded") != std::string::npos || res.second.find("/form-data") != std::string::npos)
		// 	req.isCGI = true;
		if (res.first && res.second.find("multipart/") != std::string::npos)
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
			} // info: should have a defined boundary for multipart request
		}
 		// should only be there if the request header size is equal to whole request size at this point
		// MULTISTEP REQUEST (expect a 100 continue before sending the request body)
		if ((res = req.info.get_header_var_by_name("Expect")).first && res.second == "100-continue")
		{
			req.continue_100 = READY;
			req.response_override = 100;
		}
		if (req.continue_100 && !req.multipart)
		{
			req.info._is_valid = false;
			req.response_override = 400;
			return ;
		}
	}
};

void Client::handleEvent(uint32_t revents)
{
	if (revents & (EPOLLERR | EPOLLHUP))
		throw std::exception();
	if (revents & EPOLLIN)
	{
		if (_receive() <= 0)
			throw std::exception();
		if (_request.state == READY)
		{
			if (!_request.header_parsed)
			{
				_request.info = Request(_request.raw_data);
				_request.header_parsed = true;
			}
			if (_request.info.is_valid() && _request.info.get_method() == POST)
			{
				if (!_request.post_options_set)
					setPostOptions(_request);
				if (_request.info.is_valid() && _request.continue_100 != READY && _request.current_content_size < _request.indicated_content_size)
					_request.state = INCOMPLETE;
			}
		}
	}
	if (revents & EPOLLOUT && (_request.state == READY))
	{
		respond();
		_request.reset(); // should not reset everything if we just sent a 100 continue
	}
};

void Client::handle_request(void)
{
	_request.info = Request(_request.raw_data);
};

int Client::respond(void)
{
	_response.create(_request, *_config);
	send(_fd, _response.send(), _response.get_size(), 0);
	// if (DEBUG)
	// {
	// 	std::cout << C_G_GREEN << "---------- RESPONSE ---------" << std::endl;
	// 	std::cout << (const char*)_response.status << std::endl;
	// 	std::cout << "-----------------------------" << C_RES << std::endl << std::endl;
	// }
	_response.clear();
	// if (_request.info.get_connection() != "keep-alive") // should also check if we didnt just send a 100 continue response
		// throw std::exception();
	return (0);
};
