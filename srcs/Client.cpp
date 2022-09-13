typedef enum e_state {
	ERROR = -1 , WAITING, INCOMPLETE, READY, DONE
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
	const char *data(void) const; // should return complete response buffer; wether it's an error or correct page
	size_t size(void) const; // should return response buffer size

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
	void _addEventListener(uint32_t revents)
	{
		epoll_event event;

		std::memset(&event, 0, sizeof(event));
		event.data.ptr = this;
		event.events = revents;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
			exit(-2);
	}
public:
	int				fd;
	int				server; // should take instead a pointer to a server
	std::set<Client*> *_clients;
	int				epoll_fd;
	sockaddr_in		address;
	Message			request;
	Message			response;

	Client(int epoll, int server) : epoll_fd(epoll) // should take instead of an fd to server a pointer to a server
	{
		socklen_t addr_len = sizeof(address);

		std::memset(&address, 0, addr_len);
		if ((fd = accept(server, (sockaddr*)&address, &addr_len)) < 0) {
			if (errno == EAGAIN)
				return;
			exit(-1);
		}
		_clients->insert(this);// must add this client in collection (or might not need to)
		_addEventListener(EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
	};

	virtual ~Client()
	{
		(void)disconnect();
		_clients->erase(this);
		//remove all messages
	};

	int disconnect(void)
	{
		int status = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		//remove all messages
		return status;
	};

	void handleEvent(uint32_t revents)
	{
		int recv_ret = 0;

		if (revents & (EPOLLERR | EPOLLHUP))
		{
			disconnect();// must close collection and destroy client as well as its ref in the clients collection
			return;
		}
		if (revents & EPOLLIN)
		{
			recv_ret = _receive();
			if (recv_ret <= 0) {
				disconnect(fd); // must close collection and destroy client
				return;
			}
			handle_request(); // must parse request then try to generate its response
		}
		if (revents & EPOLLOUT && recv_ret)
		{
			respond();
		}
	};
	int receive(void)
	{

	};
	int respond()
	{
		// should instead be non blocking
		// must take as last param a flag which state if more data neeeds to be sent
		send(fd, response.data(), response.size(), 0);
		// if request specified to close the connection then try and close it

	};
};
