#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void Server::launch(void) {
	Socket cluster;

	cluster.init_epoll();
	cluster.init_socket("0.0.0.0", 8080);
	// cluster.init_socket("127.0.0.3", 9090);
	cluster.event_loop();

}
