#ifndef SERVER_HPP
# define SERVER_HPP
# include <list>
# include "SimpleSocket.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server
{
	public:
		Server(unsigned int port);
		~Server();

	private:
		SimpleSocket		_socket;
		Request				_request;
		Response			_response;
};

#endif
