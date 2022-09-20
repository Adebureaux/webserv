#include "Response.hpp"

std::map<int, std::string> status_code;

void Response::create(const Request& request, const std::string& root)
{
	_status = 200;
	_root = root;
	if (!status_code.size())
		_init_status_code();
	if (!request.is_valid())
		_status = 400;
	if (request.get_method() == GET)
		_create_get(request);
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

void Response::_create_get(const Request& request)
{
	(void)request;
	File file(request.get_request_target(), _root);
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
