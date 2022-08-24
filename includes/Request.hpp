#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string>
# include <sstream>
# include <iostream>

class Request {
	public:
		Request();
		~Request();

		void fill(const std::string& header);

		std::string getMethod(void) const;
		std::string getPath(void) const;
		std::string getHttp(void) const;

		void debug(void);

	private:
		std::string			_method;
		std::string			_path;
		std::string			_http;
};

#endif
