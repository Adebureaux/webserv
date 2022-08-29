#include "Server.hpp"

void signalHandler(int signal) {
	gsocket.getClient().clear();
	exit(signal);
}

Server::Server(const std::string& address, unsigned int port) {
	signal(SIGINT, signalHandler);
	gsocket.initialize(address, port);
	while (1) {
		gsocket.waitRequest();
		for (Socket::map::iterator it = gsocket.getClient().begin(); it != gsocket.getClient().end(); it++) {
			if (gsocket.isReadSet(it->first)) { 
				if (it->first == gsocket.getServerFd())
					gsocket.acceptClient();
				else if (gsocket.communicate(it->first)) {
					_request.fill(gsocket.getHeaderRequest(it->first));
					_response.respond(_request);
					if (gsocket.isWriteSet(it->first))
						send(it->first, _response.send().c_str(), _response.send().size(), MSG_DONTWAIT);
					_request.clear();
					_response.clear();
				}
				else
					it = gsocket.getClient().begin();
			}
		}
	}
}

Server::~Server() {}
