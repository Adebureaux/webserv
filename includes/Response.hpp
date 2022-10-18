#pragma once
#include "Shared.hpp"
#include "Request.hpp"
#include "Utility.hpp"
#include "Autoindex.hpp"
#include "Conf.hpp"

class Response
{

	public:
	Response();
	Response(const Response &rhs);
	Response& operator=(const Response& rhs);
	~Response();
	void		create(Message& request, config_map& config);
	void		clear(void);
	const void*	send(void) const;
	size_t		get_size(void) const;
	void		create_get(const Message& request);
	void		create_post(Message& request, Server_block& config);
	void		create_delete(const Message& request);

	private:
	void	_find_location(const Request& request, Server_block& config);
	location_map::iterator _find_longest_location(Server_block& config, std::string path) const;
	void	_generate_response(int status);
	void	_construct_response(const Message& request, int status);
	void	_init_start_lines(void) const;
	void 	_load_errors(Server_block& config);
	void	_construct_autoindex(const std::string& filename, const std::string &pseudo_root);
	void	_construct_error(int status, bool generate);
	void	_header_field(const std::string& header, const std::string& field);
	std::string	_merge_path(const std::string& root, std::string path);
	std::string	 _parse_host(std::string host);
	void _setup_redirection(const Request& request);
	void _cgi_get(Message& request, Server_block& config);
	void _cgi_post(Message& request, Server_block& config);
	void _cgi_delete(Message& request, Server_block& config);

	private:
	Location					*_location;
	File						_file;
	std::string					_redirect;
	std::string					_response;
	std::string					_header;
	std::string					_body;
	std::map<int, std::string>	_errors;
};
