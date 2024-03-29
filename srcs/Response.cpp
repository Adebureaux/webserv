#include "Multipart.hpp"
#include "Response.hpp"

static std::map<int, std::string> start_lines;
Response::Response() :  _location(NULL), _file(), _isCGI(false)
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
	_redirect = rhs._redirect;
	_isCGI = rhs._isCGI;
	return (*this);
}

Response::~Response() {}

void Response::clear(void)
{
	_response.clear();
	_header.clear();
	_body.clear();
	_redirect.clear();
	_file = File();
	_location = NULL;
	_isCGI = false;
}

const void* Response::send(void) const
{
	return (_response.c_str());
}

size_t Response::get_size(void) const
{
	return (_response.size());
}

static bool create_filu(std::string const &content, std::string const &path)
{
	std::fstream fs;
	fs.open(path.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
	if (!fs.is_open())
		return false;
	fs << content;
	fs.close();
	return true;
}

void Response::create(Message& request, config_map& config)
{
	config_map::iterator it = config.find(_parse_host(request.info.get_host()));
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
	_load_errors(it->second);
	_find_location(request.info, it->second);

	if (!request.info.is_valid())
		_construct_error(400, true);
	else if (request.info.get_method() == NO_METHOD)
		_construct_error(405, true);
	else if (request.info.get_var_by_name("HTTP_VERSION").second != "1.1")
		_construct_error(505, true);
	else if (!_location)
		_construct_response(request, 404);
	else if (request.info.get_method() == GET)
		create_get(request, it->second);
	else if (request.info.get_method() == POST)
		create_post(request, it->second);
	else if (request.info.get_method() == DELETE)
		create_delete(request, it->second);
}

void Response::create_get(const Message& request, Server_block& config)
{
	if (_file.redirect or !_location->redirect.empty())
		_construct_response(request, 302);
	else if (!_location->get_method)
		_construct_response(request, 405);
	else if (!_file.not_found and !(_file.permissions & R))
		_construct_response(request, 403);
	else if (_file.valid and _file.type == FILE_TYPE)
	{
		if (_file.ext == "php" and !_location->CGI.empty())
		{
			_isCGI = true;
			return _cgi(request, config);
		}
		_construct_response(request, 200);
	}
	else if (_file.valid and _file.type == DIRECTORY and _location->autoindex)
		_construct_autoindex(_file.path, request.info.get_var_by_name("ABSOLUTE_PATH").second);
	else
		_construct_response(request, 404);
}

void Response::create_post(Message& request, Server_block& config)
{
	if (_file.redirect or !_location->redirect.empty())
		_construct_response(request, 302);
	else if (!_location->post_method)
		_construct_response(request, 405);
	else if (!_file.not_found and !(_file.permissions & R))
		_construct_response(request, 403);
	else if (config.body_size > 0 && request.indicated_content_size > config.body_size)
		_construct_response(request, 413);
	else if (request.continue_100 == READY)
	{
		_construct_response(request, 100);
		request.continue_100 = DONE;
	}
	else if (request.current_content_size == request.indicated_content_size and _file.ext != "php")
	{
		if (_location->upload.first)
		{
			try
			{
				Multipart moultipass(std::string(request.raw_data.begin() + request.header_size, request.raw_data.end()), request.boundary);
				std::map<std::string, File_Multipart> multipart_map = moultipass.get_files();
				std::map<std::string, File_Multipart>::iterator it = multipart_map.begin();
				std::string path = _location->upload.second;
				while (it != multipart_map.end())
				{
					std::string pathandfilename = _location->root + _location->upload.second + it->second._filename;
					if (!create_filu(it->second._file, pathandfilename))
						throw std::exception();
					if (multipart_map.size() == 1)
						_header_field("Location",  _location->uri + _location->upload.second + it->second._filename);
					else if (it == multipart_map.begin() and multipart_map.size() > 1)
						_header_field("Location",  _location->uri + _location->upload.second);
					it++;
				}
				_construct_response(request, 201);
			}
			catch (std::exception& e)
			{
				(void)e;
				_construct_response(request, 500);
			}
		}
		else _construct_response(request, 403);
	}
	else if (request.current_content_size == request.indicated_content_size and _file.ext == "php")
	{
		if (!_location->CGI.empty())
		{
			_isCGI = true;
			return _cgi(request, config);
		}
		else _construct_response(request, 403);
	}
	else
		_construct_response(request, 400);
}

void Response::create_delete(const Message &request, Server_block &config)
{
	std::cout << request.raw_data << std::endl;
	if (_file.redirect or !_location->redirect.empty())
		_construct_response(request, 302);
	else if (!_location->delete_method)
		_construct_response(request, 405);
	else if (!_file.not_found and !(_file.permissions & R))
		_construct_response(request, 403);
	else if (_file.valid and _file.type == FILE_TYPE)
	{
		if (_file.ext == "php")
		{
			if (!_location->CGI.empty())
			{
				_isCGI = true;
				return _cgi(request, config);
			}
			else
				return _construct_response(request, 403);
		}
		std::remove(_file.uri.c_str());
		_construct_response(request, 204);
	}
	else
		_construct_response(request, 404);
}

void Response::_find_location(const Request& request, Server_block& config)
{
	File requested(request.get_var_by_name("ABSOLUTE_PATH").second);
	location_map::iterator it = _find_longest_location(config, requested.path);
	if (it != config.locations.end())
	{
		_location = &it->second;
		std::string path = _merge_path(_location->root, requested.path);
		if (requested.name.empty())
		{
			File file(_location->default_file, path);
			if (file.valid and file.type == FILE_TYPE)
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



void Response::_load_errors(Server_block& config)
{
	int errorn[ERROR_NUMBER] = { 400, 403, 404, 405, 413, 500, 501 , 505 };
	std::map<int, std::string>::iterator find;
	for (int i = 0; i < ERROR_NUMBER; i++)
	{
		find = config.error_pages.find(errorn[i]);
		if (find != config.error_pages.end())
		{
			File error_file(find->second);
			if (error_file.valid and error_file.type == FILE_TYPE)
			{
				error_file.set_content();
				_errors.erase(errorn[i]);
				_errors.insert(std::make_pair(errorn[i], error_file.content));
			}
		}
	}
	_errors.insert(std::make_pair(400, ERROR_HTML_400));
	_errors.insert(std::make_pair(403, ERROR_HTML_403));
	_errors.insert(std::make_pair(404, ERROR_HTML_404));
	_errors.insert(std::make_pair(405, ERROR_HTML_405));
	_errors.insert(std::make_pair(413, ERROR_HTML_413));
	_errors.insert(std::make_pair(500, ERROR_HTML_500));
	_errors.insert(std::make_pair(501, ERROR_HTML_501));
	_errors.insert(std::make_pair(505, ERROR_HTML_505));
}

void Response::_generate_response(int status)
{
	if (status == 100)
		_response = start_lines[status];
	else
	{
		// std::cout << C_G_RED << __FUNCTION__ << __LINE__ << C_RES << std::endl;

		_response = start_lines[status];
		_response.append(_header);
		if (!_isCGI)
			_response.append("\r\n");
		_response.append(_body);
		// std::cout << C_G_RED << _response << C_RES << std::endl;

	}
}

void Response::_construct_response(const Message& request, int status)
{
	std::stringstream size;

	_header_field("Server", "webserv/1.0 (Ubuntu)");
	if (!request.info.get_connection().empty())
		_header_field("Connection", request.info.get_connection());
	if (status == 200 or status == 201)
	{
		if (_isCGI)
			size << _file.content.size() - (_file.content.find(__DOUBLE_CRLF) + 4);
		else
			size << _file.content.size();
		_header_field("Content-Type", _file.mime_type);
		_header_field("Content-Length", size.str());
		_body.append(_file.content);
	}
	else if (status == 204)
	{
		if (_isCGI)
		{
			size << _file.content.size() - (_file.content.find(__DOUBLE_CRLF) + 4);

			_header_field("Content-Type", _file.mime_type);
			_header_field("Content-Length", size.str());
		}
		// else
		// 	size << _file.content.size();
		// _header_field("Content-Type", _file.mime_type);
		// _header_field("Content-Length", size.str());
		// _body.append(_file.content);
	}
	else if (status < 400)
	{
		_setup_redirection(request.info);
		if (!_redirect.empty())
			_header_field("Location", _redirect);
		else
			_construct_error(500, false);
	}
	else if (!(status == 100))
	{
		_construct_error(status, false);
	}
	_generate_response(status);
}

void Response::_construct_autoindex(const std::string& filename, const std::string &pseudo_root)
{
	std::stringstream size;

	Autoindex autoindex(ls(filename.c_str()));
	std::pair<std::string, size_t> res = autoindex.to_html(pseudo_root);
	_header_field("Content-Type", "text/html");
	size << res.second;
	_header_field("Content-Length", size.str());
	_body.append(res.first);
	_generate_response(200);
}

void Response::_construct_error(int status, bool generate)
{
	std::stringstream size;

	size << _errors[status].size();
	_header_field("Content-Type", "text/html");
	_header_field("Content-Length", size.str());
	_body.append(_errors[status]);
	if (generate)
		_generate_response(status);
}

void Response::_header_field(const std::string& header, const std::string& field)
{
	_header.append(header);
	_header.append(": ");
	_header.append(field);
	_header.append("\r\n");
}

std::string	Response::_merge_path(const std::string& root, std::string path)
{
	std::string new_path(root);
	if (_location->uri != "/")
		path = path.substr(_location->uri.size());
	if (path != "/")
		new_path.append(path);
	return (new_path);
}

std::string Response::_parse_host(std::string host)
{
	std::size_t pos = host.find_first_of(':');

	if (pos != std::string::npos)
		host = host.substr(0, pos);
	return (host);
}

void Response::_setup_redirection(const Request& request)
{
	if (_file.type == DIRECTORY and !_file.valid)
	{
		_redirect = std::string("http://") + request.get_host() + "/" + request.get_var_by_name("ABSOLUTE_PATH").second + "/";
		return;
	}
	if (_location->redirect == _location->uri)
		return;
	if (!_location->redirect.find("http"))
	{
		if (_redirect != _location->redirect)
			_redirect = _location->redirect;
	}
	else
		_redirect = std::string("http://") + request.get_var_by_name("ABSOLUTE_PATH").second + "/" +  _location->redirect;
	_redirect = _merge_path(_redirect, request.get_var_by_name("ABSOLUTE_PATH").second);
}

void Response::_init_start_lines(void) const
{
	start_lines.insert(std::make_pair(100, "HTTP/1.1 100 Continue\n"));
	start_lines.insert(std::make_pair(200, "HTTP/1.1 200 OK\n"));
	start_lines.insert(std::make_pair(201, "HTTP/1.1 201 Created\n"));
	start_lines.insert(std::make_pair(202, "HTTP/1.1 202 Accepted\n"));
	start_lines.insert(std::make_pair(204, "HTTP/1.1 204 No content\n"));
	start_lines.insert(std::make_pair(301, "HTTP/1.1 301 Moved Permanently\n"));
	start_lines.insert(std::make_pair(302, "HTTP/1.1 302 Found\n"));
	start_lines.insert(std::make_pair(400, "HTTP/1.1 400 Bad Request\n"));
	start_lines.insert(std::make_pair(403, "HTTP/1.1 403 Forbidden\n"));
	start_lines.insert(std::make_pair(404, "HTTP/1.1 404 Not Found\n"));
	start_lines.insert(std::make_pair(405, "HTTP/1.1 405 Method Not Allowed\n"));
	start_lines.insert(std::make_pair(413, "HTTP/1.1 413 Request Entity Too Large\n"));
	start_lines.insert(std::make_pair(500, "HTTP/1.1 500 Internal Server Error\n"));
	start_lines.insert(std::make_pair(501, "HTTP/1.1 501 Not Implemented\n"));
	start_lines.insert(std::make_pair(505, "HTTP/1.1 505 HTTP Version not supported\n"));
}
