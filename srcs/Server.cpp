#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void Server::launch(void) {
	Socket socket;

	socket.init_epoll();
	socket.init_socket();
	socket.event_loop();
	/*
	 * You should write a cleaner here.
	 *
	 * Close all client file descriptors and release
	 * some resources you may have.
	 *
	 * You may also want to set interrupt handler
	 * before the event_loop.
	 *
	 * For example, if you get SIGINT or SIGTERM
	 * set `state->stop` to true, so that it exits
	 * gracefully.
	 */
}
