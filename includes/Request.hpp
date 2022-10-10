#pragma once
#include "Parser.hpp"

class Request : public Parser
{
	public:
	Request(const std::string& raw_request);
	Request(const Request &cpy);
	Request &operator=(const Request &src);
	~Request();

	int							 		get_method() const;
	void								set_raw_request(const std::string& raw_request);
	std::string const					get_raw_request() const;
	std::string const					get_connection() const;
	std::string const					get_authority() const;
	std::string const					get_host() const;
	std::string const					get_request_target() const;
	std::string const					get_message_body() const;
	std::pair<bool, std::string>		get_var_by_name(const std::string &name) const;
	std::pair<bool, std::string> 		get_header_var_by_name(const std::string &name) const;
	std::map<std::string, std::string>	get_var_map() const;
	std::map<std::string, std::string>	get_header_var_map() const;
	bool								is_valid() const;
	bool								_is_valid;

	private:

	typedef void (Request::*pf)(void);
	int		 							_method;
	size_t								_head_msg_body;
	std::map<std::string, std::string>	_header_var_map;
	std::map<std::string, std::string>	_var_map;
	std::string 						_error_msg;

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
	void n_star_m(int n, int m,void (Request::*fct)(void));		// s													// ✓

	void http_message();
		void start_line();
			void status_line();
				void status_code_phrase();
			void request_line();
				void method();
					void _get();
					void _post();
					void _delete();
				void request_target();
					void origin_form();
						void absolute_path();
							void segment();
						void query();
					void absolute_form();
						void absolute_URI();
						void hier_part();
							void path_abempty();
							void path_absolute();
								void segment_nz();
							void path_rootless();
					void authority_form();
						void authority();
							void userinfo();
							void host();
								void IP_literal();
									void IPv6address();
										void h16();
										void ls32();
									void IPvFuture();
								void IPv4address();
									void dec_octet();
								void reg_name();
							void port();
					void asterisk_form();
				void http_version();
					void http_name();
		void header_field();
			void field_name();
			void field_value();
				void field_content();
					void field_vchar();
						void VCHAR();
						void obs_text();
					void obs_fold();
		void message_body();
};
