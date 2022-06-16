#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <map>

struct Request {

	private:

		std::map<std::string, Header> _headers;
		std::vector<char> _body;
		size_t _length;
		std::string _client_ip;

		Result<bool> receive_chunked(std::vector<char> &buff);
		bool receive_raw(std::vector<char> &buff); //for now buff is a char* maybe use a vector?

	public:

		// methods::s_method method;
		// Target target;
		// Version version; //see what method, target, and version are used for...

		Request();
		//Request(methods::s_method method, Target target, Version version);
		
		void set_header(const Header &header);

		Result<std::string> get_header(const std::string &name) const;
		const std::vector<char> &get_body() const;
		std::string get_client_ip(void) const;
		void set_client_ip(std::string client_ip);

		Result<bool> receive(std::vector<char> &vector);

		friend std::ostream &operator<<(std::ostream &stream, const Request &req);

};

#endif