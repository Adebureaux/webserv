typedef enum e_state {
	ERROR = -1 , INCOMPLETE, READY, DONE
} t_state;

class Message
{
public:
	t_state			state;
	std::string		raw_data;
	std::string		header;
	std::string		body;
	Client			*client;
	Message			*ptr; // response || request
	Request			info;

	Message(Client c) :
		client(c),
		state(INCOMPLETE),
		raw_data(NULL),
		header(NULL),
		body(NULL),
		ptr(NULL),
		info(NULL)
	{};
	virtual ~Message();

};

class Client {

private:
	int _receive(void)
	{
		static char buffer[4096];
		std::stringstream res;
		int ret = 0;
		while ((ret = recv(fd, buffer, sizeof(buffer))) > 0)
		{
			res << buffer;
			std::memset(buffer, 0, ret);
		}
		request.raw_data = res.str();
		if (ret < 0)
			return ret;
		return request.raw_data.size();
	}
public:
	int				fd;
	int				server_fd;
	int				epoll_fd;
	sockaddr_in		address;
	Message			request;
	Message			response;
	Client(int epoll) : fd(fd), address(addr), epoll_fd(epoll)
	{
		socklen_t addr_len = sizeof(address);

		std::memset(&address, 0, addr_len);
		if ((fd = accept(server, (sockaddr*)&addr, &addr_len)) < 0) {
			if (errno == EAGAIN)
				return;
			_exit_error("accept failed");
		}
		_clients.insert(client_fd);
		_epoll_add(client_fd, EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
	};
	virtual ~Client();

	int close_connection(void)
	{
		int status = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
	}
	void handle_event(uint32_t revents)
	{
		int recv_ret = 0;

		if (revents & (EPOLLERR | EPOLLHUP))
		{
			_close_connection(fd);// must close collection and destroy client as well as its ref in the clients collection
			return;
		}
		if (revents & EPOLLIN)
		{
			recv_ret = _receive();
			if (recv_ret <= 0) {
				_close_connection(fd); // must close collection and destroy client
				return;
			}
			handle_request(); // must parse request then try to generate its response
		}
		if (revents & EPOLLOUT && recv_ret) {
			response.respond(request);
			send(fd, response.send().c_str(), response.send().size(), 0);
		}
	}
	int receive(void) {

	}
};
