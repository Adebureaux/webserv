#include "Response.hpp"
#include "Utility.hpp"

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
	start_lines.insert(std::make_pair(100, "HTTP/1.1 100 Continue\r\n"));
	start_lines.insert(std::make_pair(200, "HTTP/1.1 200 OK\r\n"));
	start_lines.insert(std::make_pair(400, "HTTP/1.1 400 Bad Request\r\n"));
	start_lines.insert(std::make_pair(403, "HTTP/1.1 403 Forbidden\r\n"));
	start_lines.insert(std::make_pair(404, "HTTP/1.1 404 Not Found\r\n"));
	start_lines.insert(std::make_pair(405, "HTTP/1.1 405 Method Not Allowed\r\n"));
	start_lines.insert(std::make_pair(500, "HTTP/1.1 500 Internal Server Error\r\n"));
	start_lines.insert(std::make_pair(501, "HTTP/1.1 501 Not Implemented\r\n"));
}

void Response::_create_get(const Request& request, const config_map::iterator& config)
{
	(void)config;
	std::stringstream size;
	File file(request.get_request_target().c_str(), "srcs"); // Integrate a root where to start finding
	file.get_content();
	size << file.content.size();
	// if (file.type != FILE_TYPE || !file.valid)
		// gerer cas ou le fichier est invalid
	_header.append("Content-Type: text/html"); // SETUP CONTENT-TYPE HERE
	_header.append("\r\n");
	std::cout << C_G_GREEN << _body << C_RES << std::endl;
	_header.append("Content-Length: "); // SETUP CONTENT-LENGTH HERE
	_body.append(size.str());
	_header.append("\r\n\r\n");
	_body.append(file.content);
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
