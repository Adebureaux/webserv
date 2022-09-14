#include "Server.hpp"

Server::Server(t_server_block config) : config(config) {}

Server::~Server() {}

int Server::init_socket(void)
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
	server_fd = fd;
	std::cerr << C_G_MAGENTA << "Server " << fd << " listening " << config.address << ":" <<  config.port << C_RES << std::endl;
	return (fd);
}
