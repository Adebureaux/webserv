#ifndef SERVER_HPP
# define SERVER_HPP
# include "Socket.hpp"

class Server
{
	public:
		Server(const std::string& address, unsigned int port);
		~Server();

	private:
		Socket _socket;
};

#endif
