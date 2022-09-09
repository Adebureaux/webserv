#include "Socket.hpp"

bool run = true;

// https://stackoverflow.com/questions/13568858/epoll-wait-always-sets-epollout-bit

void signal_handler(int sig) {
	if (sig == SIGINT)
		run = false;
}

Socket::Socket() {
	std::signal(SIGINT, signal_handler);
}

Socket::~Socket() {
	close(_server_fd);
	close(_epoll_fd);
}

void Socket::init_epoll(void)
{
	if ((_epoll_fd = epoll_create(true)) < 0)
		_exit_error("epoll_create failed");
}

void Socket::init_socket(const std::string& address, int port)
{
	int opt = 1;
	run = true;
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	std::cerr << "\033[1;35mCreating TCP socket...\033[0m" << std::endl;
	if ((_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP)) < 0)
		_exit_error("socket failed");

	// Socket option to reuse our address
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		_exit_error("cannot set socket option 'SO_REUSEADDR'");

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	// addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_addr.s_addr = inet_addr(address.c_str());

	if (bind(_server_fd, (sockaddr*)&addr, addr_len) < 0)
		_exit_error("bind failed");

	if (listen(_server_fd, SOMAXCONN) < 0)
		_exit_error("listen failed");
	_epoll_add(_server_fd, EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);

	std::cerr << "\033[1;35mServer " << _server_fd << " listening " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;

}

void Socket::event_loop(void)
{
	int timeout = 3000; /* in milliseconds */
	int epoll_ret;
	epoll_event events[MAX_EVENTS];

	while (run) {
		epoll_ret = epoll_wait(_epoll_fd, events, MAX_EVENTS, timeout);
		if (epoll_ret == 0)
			std::cerr << "\033[1;35mWaiting for new connection ...\033[0m" << std::endl;
		else if (epoll_ret == -1) {
			if (errno == EINTR) {
				std::cerr << std::endl << "\033[1;35mClosing websev...\033[0m" << std::endl;
				continue;
			}
			_exit_error("epoll_wait failed");
			break;
		}
		else
			std::cerr << "\033[1;35mProcessing " << epoll_ret << " connection(s)\033[0m" << std::endl;

		for (int i = 0; i < epoll_ret; i++) {
			int fd = events[i].data.fd;

			if (fd == _server_fd) {
				_accept_new_client();
				continue;
			}
			_handle_client_event(fd, events[i].events);
		}
	}
}

void Socket::_handle_client_event(int fd, uint32_t revents)
{
	char buffer[BUFFER_SIZE];
	ssize_t recv_ret;
	Request request;
	Response response;

	if (revents & (EPOLLERR | EPOLLHUP)) {
		_close_connection(fd);
		return;
	}

	if (revents & EPOLLIN) {
		recv_ret = recv(fd, buffer, sizeof(buffer), 0);
		if (recv_ret == 0) {
			_close_connection(fd);
			return;
		}
		if (recv_ret < 0) {
			if (errno == EAGAIN){
				std::cerr << "EAGAIN read triggered" << std::endl;
				return;
			}
			_close_connection(fd);
			return;
		}
		buffer[recv_ret] = '\0';
		if (buffer[recv_ret - 1] == '\n')
			buffer[recv_ret - 1] = '\0';
		request.fill(buffer);
	}

	if (revents & EPOLLOUT) {
		response.respond(request);
		send(fd, response.send().c_str(), response.send().size(), 0);
	}

}

void Socket::_close_connection(int fd) {
	std::cerr << "\033[1;35mRemove client " << fd << "\033[0m" << std::endl;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
		_exit_error("epoll_ctl failed");
	close(fd);
	_client_slot.erase(fd);
}

void Socket::_accept_new_client(void)
{
	int client_fd;
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	std::memset(&addr, 0, sizeof(addr));
	if ((client_fd = accept(_server_fd, (sockaddr*)&addr, &addr_len)) < 0) {
		if (errno == EAGAIN)
			return;
		_exit_error("accept failed");
	}
	_client_slot.insert(client_fd);
	_epoll_add(client_fd, EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
	std::cerr << "\033[1;35mCreate client " << client_fd << " " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;
}

void Socket::_epoll_add(int fd, uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.fd = fd;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		_exit_error("epoll_ctl failed");
}


void Socket::_exit_error(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	std::exit(errno);
}
