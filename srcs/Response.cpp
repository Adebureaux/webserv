#include "Response.hpp"

std::map<int, std::string> status_code;

void Response::create(const Request& request, const std::map<std::string, t_server_block>::iterator& config)
{
	_status = 200;
	if (!status_code.size())
		_init_status_code();
	if (!request.is_valid())
		_status = 400;
	else if (request.get_method() == GET)
		_create_get(request, config);
	_generate_response();
}

void Response::erase(void)
{
	_response.erase();
}

const void* Response::send(void) const
{
	return (_response.c_str());
}

size_t Response::get_size(void) const
{
	return (_response.size());
}

void Response::_init_status_code(void) const
{
	status_code.insert(std::make_pair(100, "HTTP/1.1 100 Continue\n"));
	status_code.insert(std::make_pair(200, "HTTP/1.1 200 OK\n"));
	status_code.insert(std::make_pair(400, "HTTP/1.1 400 Bad Request\n"));
	status_code.insert(std::make_pair(403, "HTTP/1.1 403 Forbidden\n"));
	status_code.insert(std::make_pair(404, "HTTP/1.1 404 Not Found\n"));
	status_code.insert(std::make_pair(405, "HTTP/1.1 405 Method Not Allowed\n"));
	status_code.insert(std::make_pair(500, "HTTP/1.1 500 Internal Server Error\n"));
	status_code.insert(std::make_pair(501, "HTTP/1.1 501 Not Implemented\n"));
}

void Response::_create_get(const Request& request, const std::map<std::string, t_server_block>::iterator& config)
{
	(void)config;
	std::ifstream file(request.get_request_target().c_str()); // Integrate a root where to start finding
	std::stringstream ssbuffer;
	std::stringstream content_size_stream;
	std::string buffer;

	ssbuffer << file.rdbuf();
	file.close();
	buffer = ssbuffer.str();
	content_size_stream << buffer.size();
	_header.append("Content-Type: text/html\n"); // SETUP CONTENT-TYPE HERE
	_header.append("Content-Length: "); // SETUP CONTENT-LENGTH HERE
	_header.append(content_size_stream.str());
	_content.append(buffer);
	std::cout << C_G_GREEN << _content << C_RES << std::endl;
}	

void Response::_generate_response(void)
{
	_response = status_code[_status];
	_response.append(_header);
	_header.erase();
	_response.append("\r\n\r\n");
	_response.append(_content);
	_content.erase();
	std::cout << C_G_GREEN << _response << C_RES;
}
