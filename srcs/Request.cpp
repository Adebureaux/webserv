#include "Request.hpp"

Request::Request() {}

Request::~Request() {}

void Request::fill(const std::string& header) {
	std::istringstream _header(header);

	std::getline(_header, _method, ' ');
	std::getline(_header, _path, ' ');
	_path.erase(0, 1);
	std::getline(_header, _http, char(13));
	debug();
}

std::string Request::getMethod(void) const {
	return (_method);
}

std::string Request::getPath(void) const {
	return (_path);
}

std::string Request::getHttp(void) const {
	return (_http);
}

void Request::debug(void) {
	if (_method == "GET")
		std::cerr << "\033[1;34mRequest : GET Method\033[0m" << std::endl;
	else if (_method == "POST")
		std::cerr << "\033[1;34mRequest : POST Method\033[0m" << std::endl;
	else if (_method == "DELETE")
		std::cerr << "\033[1;34mRequest : DELETE Method\033[0m" << std::endl;
	else
		std::cerr << "\033[1;31mRequest : Missing method in request line\033[0m" << std::endl;

	std::cerr << "\033[1;34mRequest : Path = " << _path << "\033[0m" << std::endl;

	if (_http == "HTTP/1.1")
		std::cerr << "\033[1;34mRequest : HTTP = HTTP/1.1\033[0m" << std::endl;
	else
		std::cerr << "\033[1;31mRequest : Missing HTTP/1.1 in request line\033[0m" << std::endl;

	std::cerr << std::endl;
}
