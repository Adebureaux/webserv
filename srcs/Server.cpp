#include "Server.hpp"

Server::Server() {
	int ret;
	struct tcp_state state;
	_socket.init_state(&state);
}

Server::~Server() {}
