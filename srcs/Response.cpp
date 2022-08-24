#include "Response.hpp"
# include <fstream>

Response::Response() {}

Response::~Response() {}

void Response::respond(const Request& request) {
	if (request.getMethod() == "GET")
		get(request);
	debug();
}

void Response::get(const Request &request) {
	std::ifstream file(request.getPath().c_str());
	std::stringstream ssbuffer;
	std::string buffer;

	ssbuffer << file.rdbuf();
	file.close();
	_response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
	buffer = ssbuffer.str();
	_response.append(SSTR("" << buffer.size()));
	_response.append("\n\n");
	_response.append(buffer);
}

const std::string& Response::send(void) {
	return (_response);
}

void Response::debug(void) {
	std::cerr << "\033[1;32mResponse : \n" << _response << "\033[0m" << std::endl;
}
