#ifndef SERVER_HPP
# define SERVER_HPP

#include "SimpleSocket.hpp"

class Server
{
	public:
		Server();
		~Server();

	private:
		SimpleSocket _socket;
};

#endif
