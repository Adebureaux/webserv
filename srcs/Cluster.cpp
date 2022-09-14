#include "Cluster.hpp"

bool run = true;

void signal_handler(int sig) {
	if (sig == SIGINT)
		run = false;
}

Cluster::Cluster() {
	if ((_epoll_fd = epoll_create(true)) == -1)
		std::cerr << C_B_RED << "Cannot create epoll" << C_RES << std::endl;
	std::signal(SIGINT, signal_handler);
}

Cluster::~Cluster() {}

void Cluster::parse(const std::string& file)
{
	int fd;
	(void)file;
	// Should parse the .conf here

	// Example below, hard coded part (should use file later)
	_server_number = 2;
	t_config conf[_server_number];

	conf[0].address = "0.0.0.0";
	conf[0].port = 8080;
	conf[0].server_name = "webserv.fr";

	conf[1].address = "127.0.0.1";
	conf[1].port = 9090;
	conf[1].server_name = "weebserv.fr";

	for (int i = 0; i < _server_number; i++)
	{
		Server server(conf[i]);
		fd = server.init_socket();
		_epoll_add(fd, EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
		_servers.insert(std::make_pair(fd, server));
	}
}


void Cluster::event_loop(void)
{
	int epoll_ret;
	epoll_event events[MAX_EVENTS];

	while (run) {
		epoll_ret = epoll_wait(_epoll_fd, events, MAX_EVENTS, TIMEOUT_VALUE);
		if (epoll_ret == 0)
			std::cerr << C_G_MAGENTA << "Waiting for new connection ..." << C_RES << std::endl;
		else if (epoll_ret == -1) {
			if (errno == EINTR) {
				std::cerr << std::endl << C_G_MAGENTA << "Closing websev..." << C_RES << std::endl;
				continue;
			}
			std::cerr << C_B_RED << "epoll_wait failed" << C_RES << std::endl;
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

void Cluster::_epoll_add(int fd, uint32_t revents)
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.fd = fd;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		std::cerr << C_B_RED << "Cannot add fd" << fd << " to epoll" << C_RES << std::endl;
}

int main(int ac, char **argv, char **envp)
{
	(void)ac;
	(void)argv;
	(void)envp;
	// Parse .conf file in cluster.config structure
	Cluster cluster;

	cluster.parse("config.conf");
	cluster.event_loop();
	return (0);
}
