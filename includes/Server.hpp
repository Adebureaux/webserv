#ifndef SERVER_HPP
# define SERVER_HPP
# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"

static Socket gsocket;

class Server
{
	public:
		Server(const std::string& address, unsigned int port);
		~Server();

	private:
		Request		_request;
		Response	_response;
};

#endif
