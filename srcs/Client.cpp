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

static bool	initMultipartRequest(Message &req)
{
	size_t pos = req.raw_data.find("Content-Type: multipart/"); // 24
	if (pos != std::string::npos)
	{
		req.multipart = true;
		std::cout << "\tMULTIPART REQUEST DETECTED\n\n";
		pos = req.raw_data.find("; boundary=", pos); //11
		if (pos != std::string::npos)
		{
			std::cout << "\tBOUNDARY REQUEST DETECTED\n\n";

			req.boundary = req.raw_data.substr(pos + 11, req.raw_data.find_first_of("\r\n", pos + 11) - (pos + 11));
			req.boundary.insert(0, "--");
			req.boundary_end = req.boundary;
			req.boundary_end.append("--");

		}
		else return (req.multipart = false); // request should be invalidated
	}
	else return (req.multipart = false);
	return true;
};



ssize_t Client::_receive(void)
{
	ssize_t received = 0;
	bool newRequest = _request.raw_data.empty();
	std::cout << (newRequest ? "\tNEW" : "\t") <<"\tREQUEST RECEIVED\n";
	for(;;)
	{
		char buffer[BUFFER_SIZE + 1] = { 0 };
		int ret = recv(_fd, buffer, BUFFER_SIZE, 0);
		if (ret == 0 || ret < 0)
			break;
		if (ret > 0)
		{
			// std::cout << ret << std::endl;

			_request.raw_data.append(buffer);
			received += ret;
		}
		if (ret < BUFFER_SIZE)
			break;
	}
	// the request is new
	// 		we check if Expect
	//			expect 100 continue

	if (newRequest)
	{
		if (_request.raw_data.find("Expect: 100-continue") != std::string::npos)
			_request.continue100 = READY;
		(void)initMultipartRequest(_request);
		// std::cout << "raw_data:" << _request.raw_data << "|"<<std::endl;
		// std::cout << "BOUNDARY:" << _request.boundary << "|"<<std::endl;
	}
	// must check wether or not there is a body to be received (req header Must contain Expect: 100-continue and a content-length)
	if ((_request.raw_data.find("\r\n\r\n") != std::string::npos && !_request.multipart)
	|| (_request.multipart && _request.raw_data.find(_request.boundary_end) != std::string::npos))
	{
		_request.state = READY;
		_request.continue100 = DONE;
	}
	else
		_request.state = INCOMPLETE;
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
	// if (DEBUG)
	// {
	// 	std::cout << C_G_YELLOW << "---------- REQUEST ----------" << std::endl;
	// 	std::cout << _request.raw_data << std::endl;
	// 	std::cout << "-----------------------------" << C_RES << std::endl << std::endl;
	// }
	_request.info = Request(_request.raw_data);
};

int Client::respond(void)
{
	_response.create(_request, *_config);
	send(_fd, _response.send(), _response.get_size(), 0);
	if (DEBUG)
	{
		std::cout << C_G_GREEN << "---------- RESPONSE ---------" << std::endl;
		std::cout << (const char*)_response.send() << std::endl;
		std::cout << "-----------------------------" << C_RES << std::endl << std::endl;
	}
	_response.clear();
	// if (_request.info.get_connection() != "keep-alive") // should also check if we didnt just send a 100 continue response
		// throw std::exception();
	return (0);
};
