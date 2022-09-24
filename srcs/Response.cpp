#include "Response.hpp"

// https://www.rfc-editor.org/rfc/rfc7230.html#section-3 --> reference
// HTTP-message   = start-line
//                  *( header-field CRLF )
//                  CRLF
//                  [ message-body ]

std::map<int, std::string> start_lines;
std::map<int, std::string> errors;

Response::Response() : _status(200), _response(std::string()), _header(std::string()), _body(std::string())
{
	_init_start_lines();
	_init_errors();
}

Response::~Response()
{
	_header.erase();
	_body.erase();
	_response.erase();
}

void Response::create(const Request& request, config_map& config)
{
	if (!request.is_valid())
		_status = 400;
	else
	{
		config_map::iterator it = config.find(request.get_host());
		if (it == config.end())
			it = config.begin();
		if (request.get_method() == GET)
			create_get(request, it->second);
		else if (request.get_method() == POST)
			create_post(request, it->second);
		else if (request.get_method() == DELETE)
			create_delete(request, it->second);
	}
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

void Response::create_get(const Request& request, Server_block& config)
{
	std::stringstream size;
	File file(request.get_request_target().c_str(), config.root);
	file.set_content();
	file.set_mime_type();
	if (file.type == FILE_TYPE && file.valid)
	{
		size << file.size;
		_header_field("Content-Type", file.mime_type);
		_header_field("Content-Length", size.str());
		_body.append(file.content);
	}
	else
	{
		_status = 404;
		_header_field("Content-Type", "text/html");
		_header_field("Content-Length", "68");
		_body.append(errors[_status]);
	}

}

void Response::create_post(const Request& request, Server_block& config)
{
	(void)request;
	(void)config;
}

void Response::create_delete(const Request& request, Server_block& config)
{
	(void)request;
	(void)config;
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

void Response::_init_errors(void) const
{
	errors.insert(std::make_pair(404, "<!DOCTYPE html>\n<html>\n<body>\n<center>Error 404: Not Found</center>"));
}

void Response::_generate_response(void)
{
	_response = start_lines[_status];
	_response.append(_header);
	_response.append("\r\n");
	std::cout << C_G_GREEN << _header << C_RES;
	_header.erase();
	_response.append(_body);
	_body.erase();
}

void Response::_header_field(const std::string& header, const std::string& field)
{
	_header.append(header);
	_header.append(": ");
	_header.append(field);
	_header.append("\r\n");
}
