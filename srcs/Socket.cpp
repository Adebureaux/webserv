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
	for (std::set<int>::iterator it = _servers.begin(); it != _servers.end(); it++)
		close(*it);
	// for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	// 	*it->disconnect();
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
	int fd;
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	run = true;
	if ((fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP)) < 0)
		_exit_error("socket failed");
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		_exit_error("cannot set socket option 'SO_REUSEADDR'");
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(address.c_str());
	if (bind(fd, (sockaddr*)&addr, addr_len) < 0)
		_exit_error("bind failed");

	if (listen(fd, SOMAXCONN) < 0)
		_exit_error("listen failed");
	_epoll_add(fd, EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
	_servers.insert(fd);
	std::cerr << "\033[1;35mServer " << fd << " listening " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;
}

void Socket::event_loop(void)
{
	int timeout = 30000;
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
		for (int i = 0; i < epoll_ret; i++)
		{
			if (_servers.count(events[i].data.fd)) // if event from server (aka should be a new client trying to connect)
				_clients.insert(new Client(_epoll_fd, events[i].data.fd, &_clients));
			else
				((Client*)(events[i].data.ptr))->handleEvent(events[i].events);
		}
	}
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
