#include "Client.hpp"

Message::Message(Client *c) :
	client(c),
	state(INCOMPLETE),
	raw_data("NULL"),
	header("NULL"),
	body("NULL"),
	ptr(NULL)
	// info(NULL)
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

Client::Client(int epoll, int server_fd, std::set<Client *> *clients) : epoll_fd(epoll), server(server_fd), _clients(clients), request(this), response(this)
{
	socklen_t addr_len = sizeof(address);

	std::memset(&address, 0, addr_len);
	if ((fd = accept(server, (sockaddr*)&address, &addr_len)) < 0)
	{
		if (errno == EAGAIN)
			return; // should thow an exception to force auto deletion because of new construction
		exit(-1);  // should thow an exception to force auto deletion because of new construction
	}
	std::cout << "new client with fd: " << fd << " accepted on server:" << server << std::endl;
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
	static char buffer[4096];
	std::stringstream res;
	int ret = 0;
	while ((ret = recv(fd, buffer, sizeof(buffer), 0)) > 0)
	{
		res << buffer;
		std::memset(buffer, 0, ret);
	}
	request.raw_data = res.str();
	if (ret < 0)
		return ret;
	return request.raw_data.size();
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


// void delete()
// {
// 	(void)disconnect();
// 	_clients->erase(this);
// 	//remove all messages
// };

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
		// handle_request(); // must parse request then try to generate its response
	}
	if (revents & EPOLLOUT && recv_ret)
	{
		respond();
	}
};

int Client::respond()
{
	// should instead be non blocking
	// must take as last param a flag which state if more data neeeds to be sent
	return send(fd, response.data(), response.size(), 0);
	// if request specified to close the connection then try and close it

};
