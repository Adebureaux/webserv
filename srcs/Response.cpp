#include "Response.hpp"

// https://www.rfc-editor.org/rfc/rfc7230.html#section-3 --> reference
// HTTP-message   = start-line
//                  *( header-field CRLF )
//                  CRLF
//                  [ message-body ]

std::map<int, std::string> start_lines;

void Response::create(const Request& request, const config_map::iterator& config)
{
	_status = 200;
	if (!start_lines.size())
		_init_start_lines();
	if (!request.is_valid())
		_status = 400;
	else if (request.get_method() == GET)
		_create_get(request, config);
	else if (request.get_method() == POST)
		_create_post(request, config);
	else if (request.get_method() == DELETE)
		_create_delete(request, config);
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

void Response::_init_start_lines(void) const
{
	start_lines.insert(std::make_pair(100, "HTTP/1.1 100 Continue\n"));
	start_lines.insert(std::make_pair(200, "HTTP/1.1 200 OK\n"));
	start_lines.insert(std::make_pair(400, "HTTP/1.1 400 Bad Request\n"));
	start_lines.insert(std::make_pair(403, "HTTP/1.1 403 Forbidden\n"));
	start_lines.insert(std::make_pair(404, "HTTP/1.1 404 Not Found\n"));
	start_lines.insert(std::make_pair(405, "HTTP/1.1 405 Method Not Allowed\n"));
	start_lines.insert(std::make_pair(500, "HTTP/1.1 500 Internal Server Error\n"));
	start_lines.insert(std::make_pair(501, "HTTP/1.1 501 Not Implemented\n"));
}

void Response::_create_get(const Request& request, const config_map::iterator& config)
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
	_body.append(buffer);
	std::cout << C_G_GREEN << _body << C_RES << std::endl;
}

void Response::_create_post(const Request& request, const config_map::iterator& config)
{
	(void)request;
	(void)config;
}

void Response::_create_delete(const Request& request, const config_map::iterator& config)
{
	(void)request;
	(void)config;
}

void Response::_generate_response(void)
{
	_response = start_lines[_status];
	_response.append(_header);
	_header.erase();
	_response.append("\r\n\r\n");
	_response.append(_body);
	_body.erase();
	std::cout << C_G_GREEN << _response << C_RES;
}
