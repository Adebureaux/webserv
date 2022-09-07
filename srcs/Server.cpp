#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void Server::launch(void) {
	Socket socket;

	socket.init_epoll();
	socket.init_socket();
	socket.event_loop();
}
