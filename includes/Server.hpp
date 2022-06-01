#ifndef SERVER_HPP
# define SERVER_HPP

#include "SimpleSocket.hpp"

class Server : public SimpleSocket
{
	public:
		Server();
		~Server();
};

#endif
