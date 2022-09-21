#pragma once
#include "Shared.hpp"
#include "Request.hpp"

class Response
{
	public:
	void		create(const Request& request, const std::map<std::string, t_server_block>::iterator& config);
	void		erase(void);
	const void*	send(void) const;
	size_t		get_size(void) const;

	private:
	void		_init_status_code(void) const;
	void		_create_get(const Request& request, const std::map<std::string, t_server_block>::iterator& config);
	void		_generate_response(void);

	private:
	int			_status;
	std::string _response;
	std::string	_header;
	std::string	_content;
};
