#include "Response.hpp"

// https://www.rfc-editor.org/rfc/rfc2616#section-6 --> reference
    //    Response      = Status-Line               ; Section 6.1
    //                    *(( general-header        ; Section 4.5
    //                     | response-header        ; Section 6.2
    //                     | entity-header ) CRLF)  ; Section 7.1
    //                    CRLF
    //                    [ message-body ]

static std::map<int, std::string> start_lines;
static std::map<int, std::string> errors;

Response::Response() :  _location(NULL), _file()
{
	_init_start_lines();
}

Response::Response(const Response &rhs)
{
	*this = rhs;
}

Response& Response::operator=(const Response& rhs)
{
	_location = rhs._location;
	_file = rhs._file;
	_response = rhs._response;
	_header = rhs._header;
	_body = rhs._body;
	return (*this);
}

Response::~Response() {}

void Response::create(const Request& request, config_map& config)
{
	config_map::iterator it = config.find(request.get_host());
	if (it == config.end())
	{
		for (config_map::iterator itr = config.begin(); itr != config.end(); itr++)
		{
			if (itr->second.main == true)
			{
				it = itr;
				break;
			}
		}
	}
	_init_errors(it->second);
	_find_location(request, it->second);
	if (!request.is_valid())
		_construct_response(400);
	else if (request.get_method() == GET)
		create_get(request);
	else if (request.get_method() == POST)
		create_post(request, it->second);
	else if (request.get_method() == DELETE)
		create_delete(request, it->second);
}

void Response::clear(void)
{
	_response.clear();
	_header.clear();
	_body.clear();
	_file = File();
	_location = NULL;
}

const void* Response::send(void) const
{
	return (_response.c_str());
}

size_t Response::get_size(void) const
{
	return (_response.size());
}

void Response::create_get(const Request& request)
{
	if (_location)
	{
		std::cout << C_G_BLUE << "Current location is " << _location->uri << C_RES << std::endl;
		if (!_location->get_method)
			_construct_response(405);
		else if (_file.valid && !(_file.permissions & R))
			_construct_response(403);
		else if (_file.valid && _file.type == FILE_TYPE)
			_construct_response(200);
		else if (_file.valid && _file.type == DIRECTORY && _location->autoindex)
			_construct_autoindex(_file.path, request.get_request_target()); // TO CHANGE
		else
			_construct_response(404);
	}
	else
	{
		_construct_response(404);
		std::cout << C_G_BLUE << "No Location Found" << C_RES << std::endl;
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

void Response::_find_location(const Request& request, Server_block& config)
{
	File requested(request.get_request_target());
	// std::cout << C_G_RED << "We request name = |" << requested.name << "| path = |" << requested.path << "| uri = |" << requested.uri << "|" << C_RES << std::endl;
	location_map::iterator it = _find_longest_location(config, requested.path);
	if (it != config.locations.end())
	{
		_location = &it->second;
		std::string path = _concatenate_path(_location->root, requested.path);
		if (requested.name.empty()) // --> We are looking for a directory
		{
			File file(_location->default_file, path);
			if (file.valid && file.type == FILE_TYPE && (file.permissions & R))
				_file = file;
			else
				_file = File(path);
		}
		else
			_file = File(requested.name, path);
		_file.set_content();
		_file.set_mime_type();
	}
}

location_map::iterator Response::_find_longest_location(Server_block& config, std::string path) const
{
	std::size_t pos;
	location_map::iterator it;

	while ((pos = path.find_last_of('/')) != std::string::npos)
	{
		path = path.substr(0, pos);
		if ((it = config.locations.find(path + '/')) != config.locations.end())
			return (it);
	}
	it = config.locations.find("/");
	return (it);
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


void Response::_init_errors(Server_block& config) const
{
	std::cout << C_G_BLUE << "Number of error files to load " << config.error_pages.size() << std::endl;
	errors.insert(std::make_pair(400, ERROR_HTML_400));
	errors.insert(std::make_pair(403, ERROR_HTML_403));
	errors.insert(std::make_pair(404, ERROR_HTML_404));
	errors.insert(std::make_pair(405, ERROR_HTML_405));
	errors.insert(std::make_pair(500, ERROR_HTML_500));
	errors.insert(std::make_pair(501, ERROR_HTML_501));
}

void Response::_generate_response(int status)
{
	_response = start_lines[status];
	_response.append(_header);
	_response.append("\r\n");
	_response.append(_body);
}

void Response::_construct_response(int status)
{
	std::stringstream size;

	if (status >= 400)
	{
		size << errors[status].size();
		_header_field("Content-Type", "text/html");
		_header_field("Content-Length", size.str());
		_body.append(errors[status]);
	}
	else
	{
		size << _file.content.size();
		_header_field("Content-Type", _file.mime_type);
		_header_field("Content-Length", size.str());
		_body.append(_file.content);
	}
	_generate_response(status);
}

void Response::_construct_autoindex(const std::string& filename, const std::string &pseudo_root)
{
	std::stringstream size;
	std::cout << C_G_CYAN << "in autoindex " << pseudo_root << " " << filename << C_RES << std::endl;

	Autoindex autoindex(ls(filename.c_str()));
	std::pair<std::string, size_t> res = autoindex.to_html(pseudo_root);

	_header_field("Content-Type", "text/html");
	size << res.second;
	_header_field("Content-Length", size.str());
	_body.append(res.first);
	_generate_response(200);
}

void Response::_header_field(const std::string& header, const std::string& field)
{
	_header.append(header);
	_header.append(": ");
	_header.append(field);
	_header.append("\r\n");
}

std::string	Response::_concatenate_path(const std::string& root, std::string path)
{
	std::string new_path(root);
	std::string::difference_type n = std::count(root.begin(), root.end(), '/');

	for (int i = 0; i < n; i++)
		path = path.substr(path.find_first_of('/') + 1);
	new_path.append(path);
	return (new_path);
}
