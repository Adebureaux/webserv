#pragma once
#include "Parser.hpp"
#include "Header.hpp"
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>


#define GET						 0
#define POST					 1
#define DELETE					 2
#define NO_METHOD				-1

#define CATCH_HEADER_VAR(X)		_header_var_map[X] = std::string(_raw_str.begin()+ old_head,_raw_str.begin() + _head)
#define CATCH_VAR(X)			_var_map[X] = std::string(_raw_str.begin()+ old_head,_raw_str.begin() + _head)

// Augustin
typedef struct s_location {
	bool			get_method = false;
	bool			post_method = false;
	bool			delete_method = false;
	std::string		redirect = "";
	std::string		root = "";
	bool			autoindex = false;
	std::string		default_file = "";
	std::string		CGI = "";
	std::pair<bool, std::string>	upload = std::pair<bool, std::string>(false, "");
} t_location;

typedef struct s_server_block {
	int										port = 0;			// Mandatory
	std::string								address = "";		// Mandatory
	std::string								server_names = "";	// Optional
	bool									main = false; 			// Le premier serveur pour un host:port sera le serveur par défaut pour cet host:port (ce qui signifie qu’il répondra à toutes les requêtes qui n’appartiennent pas à un autre serveur).
	std::map<int, std::string>				error_pages;	// Optional
	size_t									body_size = 0;		// Optional ? (setup default value)
	std::vector<t_location>					locations;		// Optional
} t_server_block;

class Conf : public Parser
{
	public:
	Conf(const std::string& raw_Conf);
	Conf(const Conf &cpy);
	Conf &operator=(const Conf &src);
	~Conf();

	int							 		get_method();
	void								set_raw_request(const std::string& raw_request);
	std::string const					get_raw_request();
	std::string const					get_connection();
	std::string const					get_authority();
	std::string const					get_host();
	std::string const					get_request_target();
	std::string const					get_message_body();
	std::pair<bool, std::string>		get_var_by_name(const std::string &name);
	std::pair<bool, std::string> 		get_header_var_by_name(const std::string &name);
	std::map<std::string, std::string>	get_var_map() const;
	std::map<std::string, std::string>	get_header_var_map() const;
	bool								is_valid();

	private:

	t_location	current_location
	typedef void (Conf::*pf)(void);
	std::vector<t_server_block> serv_vector;
	

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

	void catch_var_header_field(size_t old_head);
	void expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg);											// ✓
	void finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg);				// ✓
	void n_star_m(int n, int m,void (Conf::*fct)(void));		// s													// ✓

	void OB(void);
	void CB(void);
	void end_of_line(void);

	void conf(void);
	void server_block(void);
	void location_block(void);
	void path_location(void);
	void path(void);
	void location_var(void);
	void method(void);
	void autoindex(void);
	void index(void);
	void upload(void);
	void cgi(void);
	void server_name(void);
	void listen(void);
	void port(void);
	void root(void);
	void error_block(void);
	void error_var(void);
	void error_code(void);
	void host(void);
	void IPv4address(void);
	void dec_octet()
};
