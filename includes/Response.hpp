#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# define SSTR(x) static_cast<std::ostringstream&>((std::ostringstream() << std::dec << x)).str()
# include "Request.hpp"

class Response {
	public:
		Response();
		~Response();

		void respond(const Request& request);
		void get(const Request &request);
		const std::string& send(void);

		void debug(void);
	
	private:
		std::string _response;
};

#endif
