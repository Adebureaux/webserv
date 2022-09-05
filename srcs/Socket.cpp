#include "Socket.hpp"

std::set<int> gclient;

Socket::Socket() {
	// int opt = true;

	// // Socket domain and type
	// if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	// 	_perrorExit("cannot create socket");

	// // Socket option to reuse our address
	// if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
	// 	_perrorExit("cannot set socket option 'SO_REUSEADDR'");
}

void Socket::init_state(struct tcp_state *state)
{
	const size_t client_slot_num = sizeof(state->clients) / sizeof(*state->clients);
	const uint16_t client_map_num = sizeof(state->client_map) / sizeof(*state->client_map);

	for (size_t i = 0; i < client_slot_num; i++) {
		state->clients[i].is_used = false;
		state->clients[i].client_fd = -1;
	}

	for (uint16_t i = 0; i < client_map_num; i++) {
		state->client_map[i] = EPOLL_MAP_TO_NOP;
	}
}

Socket::~Socket(void) {
	close(_server_fd);
}

void signalHandler(int signal) {
	exit(signal);
}

void Socket::initialize(void) {
	std::signal(SIGINT, signalHandler);
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(8080);
	// inet_aton("127.0.0.2", &_server_addr.sin_addr);
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // This one for any addresses 
	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) 
		_perrorExit("bind failed"); 
	if (listen(_server_fd, SOMAXCONN) == -1) // Maximum number of client listned
		_perrorExit("listen failed");
}

void Socket::waitRequest(void) {
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_SET(_server_fd, &_readfds);
	FD_SET(_server_fd, &_writefds);
	for (std::set<int>::iterator it = gclient.begin(); it != gclient.end(); it++) {
		FD_SET(*it, &_readfds);
	}
	std::cerr << "\033[1;35mWaiting for new connexion ...\033[0m" << std::endl;
	if (select(_server_fd + gclient.size() + 1, &_readfds, &_writefds, NULL, NULL) == -1)
		_perrorExit("select failed");
	FD_SET(_server_fd, &_writefds); // Not correct
}

void Socket::acceptClient(void) {
	int fd;
	struct sockaddr_in addr;
	unsigned int addrlen = sizeof(sockaddr_in);

	if (FD_ISSET(_server_fd, &_readfds)) {
		if ((fd = accept(_server_fd, (struct sockaddr*)&addr, (socklen_t*)&addrlen)) == -1)
			_perrorExit("accept failed");
		gclient.insert(fd);
		std::cerr << "\033[1;35mCreate client " << fd << " " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;
	}
}

void Socket::communicate(void) {
	int rd;
	char buffer[BUFFER_SIZE];
	Request request;
	Response response;

	for (std::set<int>::iterator it = gclient.begin(); it != gclient.end(); it++) {
		if (FD_ISSET(*it, &_readfds)) {
			if (!(rd = recv(*it, buffer, BUFFER_SIZE, MSG_DONTWAIT))) {
				std::cerr << "\033[1;35mRemove client " << *it << "\033[0m" << std::endl;
				FD_CLR(*it, &_writefds);
				FD_CLR(*it, &_readfds);
				close(*it);
				gclient.erase(*it);
				it = gclient.begin();
			}
			else { // if (FD_ISSET(_server_fd, &_writefds)) { // Not correct
				buffer[rd] = '\0';
				request.fill(buffer);
				response.respond(request);
				send(*it, response.send().c_str(), response.send().size(), MSG_DONTWAIT);
				std::cerr << "\033[1;35mServing client " << *it << "\033[0m" << std::endl;
			}
		}
	}
}

void Socket::_perrorExit(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
