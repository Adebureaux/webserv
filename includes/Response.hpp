#pragma once
#include "Shared.hpp"
#include "Request.hpp"

class Response
{

	public:
	Response();
	~Response();
	void		create(const Request& request, const config_map::iterator& config);
	void		erase(void);
	const void*	send(void) const;
	size_t		get_size(void) const;
	void		create_get(const Request& request, const config_map::iterator& config);
	void		create_post(const Request& request, const config_map::iterator& config);
	void		create_delete(const Request& request, const config_map::iterator& config);

	private:
	void		_init_start_lines(void) const;
	void		_generate_response(void);
	void		_header_field(const std::string& header, const std::string& field);


	private:
	int			_status;
	std::string _response;
	std::string	_header;
	std::string	_body;
};
