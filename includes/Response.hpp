#pragma once
#include "Shared.hpp"
#include "Request.hpp"

class Response
{
	typedef void(Response::*Method)(const Request&, const config_map::iterator&);

	public:
	void		create(const Request& request, const config_map::iterator& config);
	void		erase(void);
	const void*	send(void) const;
	size_t		get_size(void) const;

	private:
	void		_init_start_lines(void) const;
	void		_create_get(const Request& request, const config_map::iterator& config);
	void		_create_post(const Request& request, const config_map::iterator& config);
	void		_create_delete(const Request& request, const config_map::iterator& config);
	void		_generate_response(void);

	// Method method[3] = { &Response::_create_get, &Response::_create_post, &Response::_create_delete };

	private:
	int			_status;
	std::string _response;
	std::string	_header;
	std::string	_body;
};
