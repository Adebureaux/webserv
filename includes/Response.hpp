#pragma once
#include "Header.hpp"
#include "Request.hpp"
#include "Autoindex.hpp"

class Response
{
	public:
	void		create(const Request& request, const std::string& root);
	void		erase(void);
	const void*	send(void) const;
	size_t		get_size(void) const;


	private:
	void		_init_status_code(void) const;
	void		_create_get(const Request& request);
	void		_generate_response(void);

	private:
	std::string	_root; // Gota change this by the root of the .conf
	int			_status;
	std::string _response;
	std::string	_header;
	std::string	_content;
};
