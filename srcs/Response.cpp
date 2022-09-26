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
		config_map::iterator it = config.find(request.get_host()); // Need to find the good location here
		// std::cout << request.get_host() << std::endl;
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

#define AUTOINDEX_ON 1

void Response::create_get(const Request& request, Server_block& config)
{
	std::stringstream size;
	File file(request.get_request_target().c_str(), config.root);
	if (file.type == FILE_TYPE && file.valid && (file.permissions & R))
	{
		file.set_content();
		file.set_mime_type();
		size << file.size;
		_header_field("Content-Type", file.mime_type);
		_header_field("Content-Length", size.str());
		_body.append(file.content);
		_status = 200;
	}
	else if (file.type == DIRECTORY && file.valid && (file.permissions & R))
	{
		std::vector<File> filelist = ls(request.get_request_target().c_str());
		File default_file;
		bool default_found = false;
		for (std::vector<File>::iterator it = filelist.begin(); it != filelist.end(); it++)
		{
			if (it->name == "index.html") // need to change config.index to location specific default fileS
			{
				default_file = *it;
				default_found = true;
				break;
			}
		}
		if (default_found && default_file.type == FILE_TYPE && file.valid && (file.permissions & R))
		{
			default_file.set_content();
			default_file.set_mime_type();
			size << default_file.size;
			_header_field("Content-Type", default_file.mime_type);
			_header_field("Content-Length", size.str());
			_body.append(default_file.content);
			_status = 200;

			return ;
		}
		if (AUTOINDEX_ON) // need to properly check this
		{
			Autoindex autoindex(filelist);
			std::pair<std::string, size_t> res = autoindex.to_html();
			_header_field("Content-Type", "text/html");
			size << res.second;
			_header_field("Content-Length", size.str());
			_body.append(res.first);
			_status = 200;

		}
		else
		{
			if (!(default_file.permissions & R))
				_status = 403;
			else
				_status = 400;
			_header_field("Content-Type", "text/html");
			size << errors[_status].size();
			_header_field("Content-Length", size.str());
			_body.append(errors[_status]);
			return ;
		}
	}
	else
	{
		// if FILE_TYPE && permissions ! R  && found -> 403
		// if not_found -> 404
		// if DIRECTORY
			// if default index is set in conf and found in the folder -> send this file
			// else if default index is not set and autoindex is on in conf -> send autoindex generated html file
		// else if invalid file (for whatever reason) -> bad request

		if (file.not_found)
			_status = 404;
		else if (!(file.permissions & R))
			_status = 403;

		else
			_status = 400;
		_header_field("Content-Type", "text/html");
		size << errors[_status].size();
		_header_field("Content-Length", size.str());
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
	start_lines.insert(std::make_pair(100, "HTTP/1.1 100 Continue\n"));
	start_lines.insert(std::make_pair(200, "HTTP/1.1 200 OK\n"));
	start_lines.insert(std::make_pair(400, "HTTP/1.1 400 Bad Request\n"));
	start_lines.insert(std::make_pair(403, "HTTP/1.1 403 Forbidden\n"));
	start_lines.insert(std::make_pair(404, "HTTP/1.1 404 Not Found\n"));
	start_lines.insert(std::make_pair(405, "HTTP/1.1 405 Method Not Allowed\n"));
	start_lines.insert(std::make_pair(500, "HTTP/1.1 500 Internal Server Error\n"));
	start_lines.insert(std::make_pair(501, "HTTP/1.1 501 Not Implemented\n"));
}

void Response::_init_errors(void) const
{
	errors.insert(std::make_pair(400, ERROR_HTML_400));
	errors.insert(std::make_pair(403, ERROR_HTML_403));
	errors.insert(std::make_pair(404, ERROR_HTML_404));
	errors.insert(std::make_pair(405, ERROR_HTML_405));
	errors.insert(std::make_pair(500, ERROR_HTML_500));
	errors.insert(std::make_pair(501, ERROR_HTML_501));
}

void Response::_generate_response(void)
{
	_response = start_lines[_status];
	_response.append(_header);
	_response.append("\r\n");
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
