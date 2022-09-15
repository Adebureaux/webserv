#include "Client.hpp"
# define SSTR(x) static_cast<std::ostringstream&>((std::ostringstream() << std::dec << x)).str()

Message::Message(Client *c) :
	client(c),
	state(INCOMPLETE),
	raw_data(""), // ! PLACEHOLDER
	// header(""),
	// body(""),
	ptr(NULL),
	info("DEFAULT")
{};

Message::~Message(){};

const char *Message::data(void) const
{
	return raw_data.c_str();// PLACEHOLDER !!
}; // should return complete response buffer; wether it's an error or correct page

size_t Message::size(void) const
{
	return raw_data.size();// PLACEHOLDER !!
}; // should return response buffer size

Client::Client(int epoll, Server& server, std::set<Client *> *clients) : epoll_fd(epoll), _server(server), _clients(clients), request(this), response(this)
{
	socklen_t addr_len = sizeof(address);

	std::memset(&address, 0, addr_len);
	if ((fd = accept(_server.server_fd, (sockaddr*)&address, &addr_len)) < 0)
	{
		if (errno == EAGAIN)
			return; // should thow an exception to force auto deletion because of new construction
		exit(-1);  // should thow an exception to force auto deletion because of new construction
	}
	std::cout << "new client with fd: " << fd << " accepted on server:" << _server.server_fd << std::endl;
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
	if ((ret = recv(fd, buffer, sizeof(buffer), 0)) > 0)
	{
		res << buffer;
		std::memset(buffer, 0, ret);
	}
	else if (ret <= 0)
		disconnect();
	request.raw_data.append(res.str());
	if (request.raw_data.find("\r\n\r\n") != std::string::npos)
	{
		request.state = READY;
	}
	if (ret < 0)
		return ret;
	// std::cout << "received request:\n" << request.raw_data << std::endl;
	return res.str().size();
}

void Client::_addEventListener(uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.ptr = this;
	event.events = revents;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		exit(-2);
	std::cout << "\tadded to epoll loop"<< std::endl;

}

int Client::disconnect(void)
{
	int status = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	//remove all messages
	return status;
};

void Client::handleEvent(uint32_t revents)
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
			disconnect(); // must close collection and destroy client
			return;
		}
	}
	if (revents & EPOLLOUT && request.state == READY)
	{
		handle_request();
		respond();
		request.raw_data.erase();
		request.state = INCOMPLETE;
	}
};

void Client::handle_request()
{
	std::cout << request.raw_data;
	request.info = Request(request.raw_data);
	std::cout << "Received request semantics : " << (request.info.is_valid() ? "valid\n" : "invalid\n");
};

int Client::respond()
{
	// should instead be sending data by chunks if necessary
	// must take as last param a flag which state if more data neeeds to be sent
	std::ifstream file("test.html");
	std::stringstream ssbuffer;
	std::stringstream content_size_stream;
	std::string buffer;

	ssbuffer << file.rdbuf();
	file.close();
	response.raw_data = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
	buffer = ssbuffer.str();
	content_size_stream << buffer.size();
	response.raw_data.append(content_size_stream.str());
	response.raw_data.append("\n\n");
	response.raw_data.append(buffer);
	// response.raw_data = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 0\n\n";
	std::cout << response.data() << std::endl;
	std::cout << "send_status: " << write(fd, response.data(), response.size()) << std::endl;
	return (0);
	// if request specified to close the connection then try and close it

};
