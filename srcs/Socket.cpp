#include "Socket.hpp"

bool run = true;

void signal_handler(int sig) {
	if (sig == SIGINT)
		run = false;
}

Socket::Socket() {
	std::signal(SIGINT, signal_handler);
}

Socket::~Socket() {}

void Socket::init_state(void)
{
	const size_t client_slot_num = sizeof(_clients) / sizeof(*_clients);
	// const uint16_t client_map_num = sizeof(_client_map) / sizeof(*_client_map);

	std::cout << client_slot_num << std::endl;
	// std::cout << client_map_num << std::endl;
	

	for (size_t i = 0; i < client_slot_num; i++) {
		_clients[i].is_used = false;
		_clients[i].client_fd = -1;
	}

	// for (uint16_t i = 0; i < client_map_num; i++) {
	// 	_client_map[i] = EPOLL_MAP_TO_NOP;
	// }
}

void Socket::init_epoll(void)
{
	std::cerr << "\033[1;35mInitializing epoll...\033[0m" << std::endl;
	if ((_epoll_fd = epoll_create(true)) < 0)
		_exit_error("epoll_create");
}

int Socket::init_socket(void)
{
	std::cerr << "\033[1;35mInitializing socket...\033[0m" << std::endl;
	int opt = 1;
	run = true;
	int ret;
	int err;
	int tcp_fd = -1;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	const char *bind_addr = "0.0.0.0";
	uint16_t bind_port = 8080;

	std::cerr << "\033[1;35mCreating TCP socket...\033[0m" << std::endl;
	tcp_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (tcp_fd < 0) {
		err = errno;
		printf("socket(): " PRERF, PREAR(err));
		return -1;
	}

	// Socket option to reuse our address
	if (setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		_exit_error("cannot set socket option 'SO_REUSEADDR'");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(bind_port);
	addr.sin_addr.s_addr = inet_addr(bind_addr);

	ret = bind(tcp_fd, (struct sockaddr*)&addr, addr_len);
	if (ret < 0) {
		ret = -1;
		err = errno;
		printf("bind(): " PRERF, PREAR(err));
		goto out;
	}

	ret = listen(tcp_fd, 10);
	if (ret < 0) {
		ret = -1;
		err = errno;
		printf("listen(): " PRERF, PREAR(err));
		goto out;
	}

	/*
	 * Add `tcp_fd` to epoll monitoring.
	 *
	 * If epoll returned tcp_fd in `events` then a client is
	 * trying to connect to us.
	 */
	ret = _my_epoll_add(_epoll_fd, tcp_fd, EPOLLIN | EPOLLPRI);
	if (ret < 0) {
		ret = -1;
		goto out;
	}


	std::cerr << "\033[1;35mListening " << bind_addr << ":" << bind_port << "\033[0m" << std::endl;
	_server_fd = tcp_fd;
	return 0;
out:
	close(tcp_fd);
	return ret;
}

int Socket::event_loop(void)
{
	int err;
	int ret = 0;
	int timeout = 3000; /* in milliseconds */
	int maxevents = 32;
	int epoll_ret;
	struct epoll_event events[32];

	printf("Entering event loop...\n");

	while (run) {

		/*
		 * I sleep on `epoll_wait` and the kernel will wake me up
		 * when event comes to my monitored file descriptors, or
		 * when the timeout reached.
		 */
		epoll_ret = epoll_wait(_epoll_fd, events, maxevents, timeout);


		if (epoll_ret == 0) {
			/*
			 *`epoll_wait` reached its timeout
			 */
			printf("I don't see any event within %d milliseconds\n", timeout);
			continue;
		}


		if (epoll_ret == -1) {
			err = errno;
			if (err == EINTR) {
				printf("Something interrupted me!\n");
				continue;
			}

			/* Error */
			ret = -1;
			printf("epoll_wait(): " PRERF, PREAR(err));
			break;
		}


		for (int i = 0; i < epoll_ret; i++) {
			int fd = events[i].data.fd;

			if (fd == _server_fd) {
				/*
				 * A new client is connecting to us...
				 */
				if (_accept_new_client(fd) < 0) {
					ret = -1;
					goto out;
				}
				continue;
			}

			/*
			 * We have event(s) from client, let's call `recv()` to read it.
			 */
			_handle_client_event(fd, events[i].events);
		}
	}

out:
	return ret;
}

void Socket::_handle_client_event(int client_fd, uint32_t revents)
{
	int err;
	ssize_t recv_ret;
	char buffer[1024];
	const uint32_t err_mask = EPOLLERR | EPOLLHUP;
	/*
	 * Read the mapped value to get client index.
	 */
	// std::cout << "INDEX = " << _client_map[client_fd] - EPOLL_MAP_SHIFT << std::endl;
	std::cout << "INDEX = " << _client[client_fd] - EPOLL_MAP_SHIFT << std::endl;


	//uint32_t index = _client_map[client_fd] - EPOLL_MAP_SHIFT;
	uint32_t index = _client[client_fd] - EPOLL_MAP_SHIFT;

	struct client_slot *client = &_clients[index];

	if (revents & err_mask)
		goto close_conn;

	recv_ret = recv(client_fd, buffer, sizeof(buffer), 0);
	if (recv_ret == 0)
		goto close_conn;

	if (recv_ret < 0) {
		err = errno;
		if (err == EAGAIN)
			return;

		/* Error */
		printf("recv(): " PRERF, PREAR(err));
		goto close_conn;
	}



	/*
	 * Safe printing
	 */
	buffer[recv_ret] = '\0';
	if (buffer[recv_ret - 1] == '\n') {
		buffer[recv_ret - 1] = '\0';
	}

	std::cout << "> " << buffer << std::endl;

	// printf("Client %s:%u sends: \"%s\"\n", client->src_ip, client->src_port,
	// 	   buffer);
	return;


close_conn:
	printf("Client %s:%u has closed its connection\n", client->src_ip,
		   client->src_port);
	_my_epoll_delete(_epoll_fd, client_fd);
	close(client_fd);
	client->is_used = false;
	return;
}

int Socket::_accept_new_client(int tcp_fd)
{
	int err;
	int client_fd;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	uint16_t src_port;
	const char *src_ip;
	char src_ip_buf[sizeof("xxx.xxx.xxx.xxx")];
	const size_t client_slot_num = sizeof(_clients) / sizeof(*_clients);


	memset(&addr, 0, sizeof(addr));
	client_fd = accept(tcp_fd, (struct sockaddr *)&addr, &addr_len);
	if (client_fd < 0) {
		err = errno;
		if (err == EAGAIN)
			return 0;

		/* Error */
		printf("accept(): " PRERF, PREAR(err));
		return -1;
	}

	src_port = ntohs(addr.sin_port);
	src_ip   = _convert_addr_ntop(&addr, src_ip_buf);
	if (!src_ip) {
		printf("Cannot parse source address\n");
		goto out_close;
	}


	/*
	 * Find unused client slot.
	 *
	 * In real world application, you don't want to iterate
	 * the whole array, instead you can use stack data structure
	 * to retrieve unused index in O(1).
	 *
	 */
	for (size_t i = 0; i < client_slot_num; i++) {
		struct client_slot *client = &_clients[i];

		if (!client->is_used) {
			/*
			 * We found unused slot.
			 */

			client->client_fd = client_fd;
			memcpy(client->src_ip, src_ip_buf, sizeof(src_ip_buf));
			client->src_port = src_port;
			client->is_used = true;
			client->my_index = i;

			/*
			 * We map the client_fd to client array index that we accept
			 * here.
			 */
			// _client_map[client_fd] = client->my_index + EPOLL_MAP_SHIFT;

			_client.insert(std::make_pair(client_fd, client->my_index + EPOLL_MAP_SHIFT));
			// std::cout << client_fd << " --> " << _client_map[client_fd] << std::endl;

			/*
			 * Let's tell to `epoll` to monitor this client file descriptor.
			 */
			_my_epoll_add(_epoll_fd, client_fd, EPOLLIN | EPOLLPRI);

			printf("Client %s:%u has been accepted!\n", src_ip, src_port);
			return 0;
		}
	}
	printf("Sorry, can't accept more client at the moment, slot is full\n");


out_close:
	close(client_fd);
	return 0;
}

const char *Socket::_convert_addr_ntop(struct sockaddr_in *addr, char *src_ip_buf)
{
	int err;
	const char *ret;
	in_addr_t saddr = addr->sin_addr.s_addr;

	ret = inet_ntop(AF_INET, &saddr, src_ip_buf, sizeof("xxx.xxx.xxx.xxx"));
	if (ret == NULL) {
		err = errno;
		err = err ? err : EINVAL;
		printf("inet_ntop(): " PRERF, PREAR(err));
		return NULL;
	}

	return ret;
}

int Socket::_my_epoll_delete(int epoll_fd, int fd)
{
	int err;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
		err = errno;
		printf("epoll_ctl(EPOLL_CTL_DEL): " PRERF, PREAR(err));
		return -1;
	}
	return 0;
}

int Socket::_my_epoll_add(int epoll_fd, int fd, uint32_t events)
{
	int err;
	struct epoll_event event;

	/* Shut the valgrind up! */
	memset(&event, 0, sizeof(event));

	event.events  = events;
	event.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
		err = errno;
		printf("epoll_ctl(EPOLL_CTL_ADD): " PRERF, PREAR(err));
		return -1;
	}
	return 0;
}

void Socket::_exit_error(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
