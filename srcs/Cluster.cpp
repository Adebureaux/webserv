#include "Cluster.hpp"

bool run = true;

void signal_handler(int sig) {
	if (sig == SIGINT)
		run = false;
}

Cluster::Cluster(server_map& config)
{
	int fd;

	if ((_epoll_fd = epoll_create(true)) == -1)
		std::cerr << C_B_RED << "Cannot create epoll" << C_RES << std::endl;
	std::signal(SIGINT, signal_handler);
	for (server_map::const_iterator it = config.begin(); it != config.end(); it++)
	{
		config_map::const_iterator it_m = it->second.begin();
		fd = _init_socket(it_m->second);
		_add_server(fd, EPOLLOUT | EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLET);
		for (config_map::const_iterator it_m = it->second.begin(); it_m != it->second.end(); it_m++)
			_servers[fd][it_m->second.server_names] = it_m->second;
	}
	config.clear();
}

Cluster::~Cluster()
{
	if (_clients.empty())
		return;
	while (1)
	{
		std::set<Client*>::iterator it = _clients.begin();
		if (it != _clients.end())
		{
			Client *ptr = *it;
			delete ptr;
		}
		else break;
	}
	for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		delete *it;
	_clients.clear();
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
			server_map::iterator it = _servers.find(events[i].data.fd);
			if (it != _servers.end())
			{
				Client *client = new Client(_epoll_fd, it->first, &it->second, &_clients);
				_clients.insert(client);
			}
			else
			{
				try
				{
					((Client*)(events[i].data.ptr))->handleEvent(events[i].events);
				}
				catch (const std::exception& e)
				{
					(void)e;
					delete (Client*)(events[i].data.ptr);
				}
			}
		}
	}
}

void Cluster::_add_server(int fd, uint32_t revents) const
{
	epoll_event event;

	std::memset(&event, 0, sizeof(event));
	event.data.fd = fd;
	event.events = revents;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		std::cerr << C_B_RED << "Cannot add fd" << fd << " to epoll" << C_RES << std::endl;
}

int Cluster::_init_socket(const Server_block& config) const
{
	int fd;
	int opt = 1;
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	if ((fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP)) == -1)
		std::cerr << C_B_RED << "Cannot create socket" << C_RES << std::endl;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		std::cerr << C_B_RED << "Cannot change socket options" << C_RES << std::endl;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config.port);
	addr.sin_addr.s_addr = inet_addr(config.address.c_str());
	if (bind(fd, (sockaddr*)&addr, addr_len) == -1)
		std::cerr << C_B_RED << "Cannot bind " << config.address << ":" << config.port << C_RES << std::endl;
	if (listen(fd, SOMAXCONN) == -1)
		std::cerr << C_B_RED << "Cannot listen " << config.address << ":" << config.port << C_RES << std::endl;
	std::cerr << C_G_MAGENTA << "Server " << fd << " listening " << config.address << ":" <<  config.port << C_RES << std::endl;
	return (fd);
}

int main(int ac, char **argv, char **envp)
{
	(void)envp;
	if (ac < 2)
	{
		std::cerr << C_G_RED << "Missing argument: configuration file" << C_RES << std::endl;
		return (1);
	}
	File conf(argv[1], "");
	if (!conf.valid || conf.type != FILE_TYPE)
	{
		std::cerr << C_G_RED << "Configuration file is not valid" << C_RES << std::endl;
		return (1);
	}
	conf.set_content();
	conf.set_mime_type();
	if (conf.mime_type != "webserv/conf")
	{
		std::cerr << C_G_RED << "Configuration file extension should be .conf" << C_RES << std::endl;
		return (1);
	}

	Conf config(conf.content);
	if (!config.is_valid())
	{
		std::cerr << C_G_RED << "Configuration file is not valid" << C_RES << std::endl;
		return (1);
	}
	Cluster cluster(config.get_conf_map());
	cluster.event_loop();
	return (0);
}
