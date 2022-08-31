#include "Socket.hpp"

map	gclient;

Socket::Socket() {
	int opt = true;

	// Socket domain and type
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		_perrorExit("cannot create socket");

	// Socket option to reuse our address
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		_perrorExit("cannot set socket option 'SO_REUSEADDR'");
}

Socket::~Socket(void) {
	close(_server_fd);
}

void signalHandler(int signal) {
	exit(signal);
}

void Socket::initialize(const std::string& address, unsigned int port) {
	signal(SIGINT, signalHandler);
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	inet_aton(address.c_str(), &_server_addr.sin_addr);
	// _server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // This one for any addresses 
	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) 
		_perrorExit("bind failed"); 
	if (listen(_server_fd, SOMAXCONN) == -1) // Maximum number of client listned
		_perrorExit("listen failed");
}

void Socket::waitRequest(void) {
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	FD_SET(_server_fd, &_readfds);
	for (map::iterator it = gclient.begin(); it != gclient.end(); it++) {
		FD_SET(it->first, &_readfds);
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
		gclient.insert(std::make_pair(fd, addr));
		std::cerr << "\033[1;35mCreate client " << fd << " " << inet_ntoa(addr.sin_addr) << ":" <<  ntohs(addr.sin_port) << "\033[0m" << std::endl;
	}
}

void Socket::communicate(void) {
	int rd;
	char buffer[BUFFER_SIZE];
	Request request;
	Response response;

	for (map::iterator it = gclient.begin(); it != gclient.end(); it++) {
		if (FD_ISSET(it->first, &_readfds)) {
			if (!(rd = recv(it->first, buffer, BUFFER_SIZE, MSG_DONTWAIT))) {
				std::cerr << "\033[1;35mRemove client " << it->first << " " << inet_ntoa(it->second.sin_addr) << ":" <<  ntohs(it->second.sin_port) << "\033[0m" << std::endl;
				// FD_CLR(it->first, &_writefds);
				// FD_CLR(it->first, &_readfds);
				close(it->first);
				gclient.erase(it->first);
				it = gclient.begin();
			}
			else if (FD_ISSET(_server_fd, &_writefds)) { // Not correct
				buffer[rd] = '\0';
				request.fill(buffer);
				response.respond(request);
				send(it->first, response.send().c_str(), response.send().size(), MSG_DONTWAIT);
				std::cerr << "\033[1;35mServing client " << it->first << " " << inet_ntoa(it->second.sin_addr) << ":" <<  ntohs(it->second.sin_port) << "\033[0m" << std::endl;
			}
		}
	}
}

void Socket::_perrorExit(const std::string& err) const {
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~Socket();
	exit(errno);
}
