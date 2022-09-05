#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void Server::launch(void) {
	Socket socket;
	// int ret;

	socket.init_state();

	socket.init_epoll();
	// if (ret != 0)
	// 	goto out;


	socket.init_socket();
	// if (ret != 0)
	// 	goto out;




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
