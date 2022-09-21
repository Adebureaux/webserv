#pragma once
#include "../../includes/Parser.hpp"
#include "Header.hpp"
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>

/*
// typedef struct s_location {
	// bool								get_method;
	// bool								post_method;
	// bool								delete_method;
	// std::string						redirect;
	// bool								autoindex;
	// std::string						root; mandatory
	// std::string						default_file; opt si autoindex true
	// std::string						CGI;
	// std::pair<bool, std::string>		upload;
} t_location;
*/

// class Server_block {
// 	public:
// 	Server_block();
// 	Server_block(const Server_block &cpy);
// 	~Server_block();
// 	Server_block &operator=(const Server_block &cpy);

	// int										port;			// Mandatory
	// std::string								address;		// Mandatory
	// std::string								server_names;	// Optional
	// bool									main; 			// Le premier serveur pour un host:port sera le serveur par défaut pour cet host:port (ce qui signifie qu’il répondra à toutes les requêtes qui n’appartiennent pas à un autre serveur).
	// std::map<int, std::string>				error_pages;	// Optional
	// size_t									body_size;		// Optional ? (setup default value)
	// std::vector<Location>					locations;		// Optional
// };

class Conf : public Parser
{
	public:
	Conf(const std::string& raw_Conf);
	Conf(const Conf &cpy);
	Conf &operator=(const Conf &src);
	~Conf();

	std::string const									get_raw_conf();
	bool												is_valid();
	std::map<int, std::map<std::string, Server_block> > get_conf_map(void);
	std::string											get_error_msg(void);

	private:

	Location		_current_location;
	Server_block	_current_server_block;
	typedef void (Conf::*pf)(void);
	std::vector<Server_block> _serv_vector;
	int				_current_error_code;
	bool			_catch_method;
	std::string		_error_msg;
	bool			_is_valid;
	std::map<int, std::map<std::string, Server_block> > _ret;


	// std::map<int, std::map<std::string, Server_block> >
	// std::vector<std::map<std::string, Server_block> >
	/*
	std::map<int, std::map<std::string, Server_block> >;

	*/
// fsefsef[8080]
	/*
	fct_pointer_tag
	s/S rule *
	o	rule |
	a	rule ' '
	n	normal fct_pointer
	r	_range
	R	_is_str
	c	_is_char
	C	_is_charset
	*/

	void _test_validity_block(void) const;
	void _check_locations(std::vector<Location> const &locations) const;
	void _create_ret_map(void);
	void catch_var_header_field(size_t old_head);
	void expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg);											// ✓
	void finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg);				// ✓
	void n_star_m(int n, int m,void (Conf::*fct)(void));		// s													// ✓

	void OB(void);
	void CB(void);
	void end_of_line(void);

	void _remove_comment(void);
	void conf(void);
	void server_block(void);
	void server_var(void);
	void location_block(void);
	// void path_location(void);
	void location_var(void);
	void location_catch_var(void);
	void path(void);
	void redirect(void);
	void method(void);
	void catch_method(void);
	void autoindex(void);
	void index(void);
	void upload(void);
	void upload_value(void);
	void cgi(void);
	void server_name(void);
	void server_name_value(void);
	void listen(void);
	void port(void);
	void root(void);
	void error_block(void);
	void error_var(void);
	void error_code(void);
	void host(void);
	void IPv4address(void);
	void dec_octet();
	void body_size(void);
	void body_size_value(void);
};
