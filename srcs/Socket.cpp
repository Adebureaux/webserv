#include "Socket.hpp"

bool run = true;

void signal_handler(int sig) {
	if (sig == SIGINT)
		run = false;
}

Socket::Socket() {
	std::signal(SIGINT, signal_handler);
}

Socket::~Socket() {
	close(_server_fd);
}

void Socket::init_epoll(void)
{
	//std::cerr << "\033[1;35mInitializing epoll...\033[0m" << std::endl;
	if ((_epoll_fd = epoll_create(true)) < 0)
		_exit_error("epoll_create failed");
}

int Socket::init_socket(void)
{
	int opt = 1;
	run = true;
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	std::cerr << "\033[1;35mCreating TCP socket...\033[0m" << std::endl;
	_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_server_fd < 0)
		_exit_error("socket failed");

	// Socket option to reuse our address
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		_exit_error("cannot set socket option 'SO_REUSEADDR'");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if (bind(_server_fd, (sockaddr*)&addr, addr_len) < 0)
		_exit_error("bind failed");

	if (listen(_server_fd, SOMAXCONN) < 0)
		_exit_error("listen failed");
	/*
	 * Add `_server_fd` to epoll monitoring.
	 *
	 * If epoll returned _server_fd in `events` then a client is
	 * trying to connect to us.
	 */
	_epoll_add(_server_fd, EPOLLIN | EPOLLPRI);

	std::cerr << "\033[1;35mServer " << _server_fd << " listening " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;
	return 0;

}

int Socket::event_loop(void)
{
	int timeout = 3000; /* in milliseconds */
	int epoll_ret;
	epoll_event events[MAX_EVENTS];

	while (run) {
		epoll_ret = epoll_wait(_epoll_fd, events, MAX_EVENTS, timeout);
		if (epoll_ret == 0) {
			std::cerr << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
		}


		if (epoll_ret == -1) {
			if (errno == EINTR) {
				std::cerr << std::endl << "\033[1;35mClosing websev...\033[0m" << std::endl;
				continue;
			}

			/* Error */
			_exit_error("epoll_wait failed");
			break;
		}


		for (int i = 0; i < epoll_ret; i++) {
			int fd = events[i].data.fd;

			if (fd == _server_fd) {
				/*
				 * A new client is connecting to us...
				 */
				_accept_new_client();
				continue;
			}
			/*
			 * We have event(s) from client, let's call `recv()` to read it.
			 */
			_handle_client_event(fd, events[i].events);
		}
	}
	return (0);
}

void Socket::_handle_client_event(int client_fd, uint32_t revents)
{
	int err;
	ssize_t recv_ret;
	char buffer[1024];
	const uint32_t err_mask = EPOLLERR | EPOLLHUP;

	if (revents & err_mask) {
		_close_connection(client_fd);
		return;
	}

	recv_ret = recv(client_fd, buffer, sizeof(buffer), 0);
	if (recv_ret == 0) {
		_close_connection(client_fd);
		return;
	}

	if (recv_ret < 0) {
		err = errno;
		if (err == EAGAIN)
			return;

		/* Error */
		printf("recv(): " PRERF, PREAR(err));
		_close_connection(client_fd);
		return;
	}



	/*
	 * Safe printing
	 */
	buffer[recv_ret] = '\0';
	if (buffer[recv_ret - 1] == '\n') {
		buffer[recv_ret - 1] = '\0';
	}

	std::cout << "> " << buffer << std::endl;

	// send(client_fd, buffer, sizeof(buffer), 0);

	// printf("Client %s:%u sends: \"%s\"\n", client->src_ip, client->src_port,
	// 	   buffer);


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

	memset(&addr, 0, sizeof(addr));
	if ((client_fd = accept(_server_fd, (sockaddr*)&addr, &addr_len)) < 0) {
		if (errno == EAGAIN)
			return;
		_exit_error("accept failed");
	}
	_client_slot.insert(std::make_pair(client_fd, addr));
	_epoll_add(client_fd, EPOLLIN | EPOLLPRI);
}

void Socket::_epoll_add(int fd, uint32_t events)
{
	epoll_event event;

	memset(&event, 0, sizeof(event));
	event.events  = events;
	event.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		_exit_error("epoll_ctl failed");
}

void Socket::_exit_error(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
