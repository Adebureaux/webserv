#ifndef SERVER_HPP
# define SERVER_HPP
# include "Socket.hpp"

class Server
{
	public:
		Server();
		~Server();

	private:
		Socket _socket;
};

#endif
