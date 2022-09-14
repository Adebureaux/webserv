#pragma once
#include <string>
#include <cstdarg>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <map>
#include <cstring>

#define GET			 0
#define POST			1
#define DELETE		  2
#define NO_METHOD	  -1

#define OR			  1
#define AND			 0

#define STAR_NO_MIN	 0
#define STAR_NO_MAX	-1
#define EXECP		   std::invalid_argument(std::string(__FUNCTION__) + "/"+ std::string(e.what()))

#define CATCH_VAR(X)	   _var_map[X] = std::string(_raw_request.begin()+ old_head,_raw_request.begin() + _head)

class Request
{
	public:
	Request(const std::string& raw_request);
	~Request();

	int							 get_method();
	void							set_raw_request(const std::string& raw_request);
	std::string const			   get_raw_request();
	std::string const			   get_connection();
	std::string const			   get_authority();
	std::string const			   get_host();
	std::string const			   get_request_target();
	std::string const			   get_message_body();
	std::pair<bool, std::string>	get_var_by_name(const std::string &name);
	std::map<std::string, std::string>   get_map() const;

	private:

	typedef void (Request::*pf)(void);
	std::string _raw_request;
	size_t	  _head;
	int		 _method;
	size_t	  _head_msg_body;
	std::map<std::string, std::string> _var_map;

	/*
	fct_pointer_tag
	s/S rule *
	o   rule |
	a   rule ' '
	n   normal fct_pointer
	r   _range
	R   _is_str
	c   _is_char
	C   _is_charset
	*/

	void ft_va_copy(va_list *dest, va_list *src);																		// ✓
	void expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg);							  // ✓
	void finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg);
	void _range(char start, char end);																	  // ✓
	void _is_char(char c);																				  // ✓
	void _is_charset(std::string const &charset);														   // ✓
	void _is_str(std::string const &str);																   // ✓
	void n_star_m(int n, int m,void (Request::*fct)(void));  //s											// ✓
	void n_star_m_or(int n, int m, ...);																	// ✓
	void n_star_m_or(int n, int m, va_list *arg);	//S													// ✓
	void n_star_m_and(int n, int m, ...);																   // ✓
	void n_star_m_and(int n, int m, va_list *arg);   //S													// ✓
	void _or(const std::string &param, ... );	   //o													 // ✓
	void _or(const std::string &param, va_list *arg);													   // ✓
	void _and(const std::string &param, ... );	  //a													 // ✓
	void _and(const std::string &param, va_list *arg);													  // ✓

	void DIGIT();			   // ✓
	void BIT();				 // ✓
	void UPALPHA();			 // ✓
	void LOALPHA();			 // ✓
	void ALPHA();			   // ✓
	void CHAR();
	void OCTET();
	void CR();				  // ✓
	void LF();				  // ✓
	void CRLF();				// ✓
	void SP();				  // ✓
	void HTAB();				// ✓
	void WSP();
	void CTL();
	void OWS();
	void DQUOTE();
	void token();
	void HEXDIG();  // ✓
	void tchar();
	void scheme();
	void PCHAR();			   // ✓
		void unreserved();
		void pct_encoded();
		void sub_delims();

	void		catch_var_header_field(size_t old_head);

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
