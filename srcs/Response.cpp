#include "Response.hpp"
#include "Utility.hpp"

// https://www.rfc-editor.org/rfc/rfc7230.html#section-3 --> reference
// HTTP-message   = start-line
//                  *( header-field CRLF )
//                  CRLF
//                  [ message-body ]

typedef void(Response::*Method)(const Request&, const config_map::iterator&);
Method method[3] = { &Response::create_get, &Response::create_post, &Response::create_delete };
std::map<int, std::string> start_lines;

Response::Response() : _status(200), _response(std::string()), _header(std::string()), _body(std::string())
{
	if (!start_lines.size())
		_init_start_lines();
}

Response::~Response()
{
	_header.erase();
	_body.erase();
	_response.erase();
}

void Response::create(const Request& request, const config_map::iterator& config)
{
	// std::cout << "server names = " << config->second.server_names << std::endl; 
	if (!request.is_valid())
		_status = 400;
	(this->*method[request.get_method()])(request, config);
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

void Response::create_get(const Request& request, const config_map::iterator& config)
{
	(void)config;
	std::stringstream size;
	File file(request.get_request_target().c_str(), ""); // Integrate a root where to start finding
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
		_status = 404; 

}

void Response::create_post(const Request& request, const config_map::iterator& config)
{
	(void)request;
	(void)config;
}

void Response::create_delete(const Request& request, const config_map::iterator& config)
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

void Response::_generate_response(void)
{
	_response = start_lines[_status];
	_response.append(_header);
	_response.append("\r\n");
	std::cout << C_G_GREEN << _header << C_RES;
	_header.erase();
	_response.append(_body);
	_body.erase();
	//std::cout << C_G_GREEN << _response << C_RES;
}

void Response::_header_field(const std::string& header, const std::string& field)
{
	_header.append(header);
	_header.append(": ");
	_header.append(field);
	_header.append("\r\n");
}
