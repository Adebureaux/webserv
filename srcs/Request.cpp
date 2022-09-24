#include "Request.hpp"

Request::Request(const std::string& raw_request)
: Parser(raw_request), _method(NO_METHOD), _head_msg_body(0), _header_var_map(), _var_map(), _error_msg(), _is_valid(false)
{
	try
	{

		http_message();
		_is_valid = true;
	}
	catch(const std::exception& e)
	{
		_error_msg = e.what();
	}
}

Request::Request(const Request &cpy)
: Parser::Parser()
{
	*this = cpy;
}

Request &Request::operator=(const Request &src)
{
	_raw_str = src._raw_str;
	_head = src._head;
	_method = src._method;
	_head_msg_body = src._head_msg_body;
	_header_var_map = src._header_var_map;
	_var_map = src._var_map;
	_error_msg = src._error_msg;
	_is_valid = src._is_valid;

	return (*this);
}

Request::~Request(){}

bool Request::is_valid() const
{
	return(_is_valid);
}

void Request::n_star_m(int n, int m, void(Request::*fct)(void))
{
	n_star_m_or(n, m, "n", fct);
}

void Request::finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg)
{
	int i;
	char *str;
	std::string string;
	void(Request::*pf)(void);

	while ( start != end)
	{
		switch (*start)
		{
			case 's':
				i = va_arg(*arg, int);
				i = va_arg(*arg, int);
				(void)(va_arg(*arg,void(Request::*)(void)));
				(void)pf;
				break;
			case 'S':
				i = va_arg(*arg, int);
				i = va_arg(*arg, int);
				i = va_arg(*arg, int);
				string = std::string(va_arg(*arg, char *));
				finish_expand(string.begin(), string.end(), arg);
				break;
			case 'o':
				string = std::string(va_arg(*arg, char *));
				finish_expand(string.begin(), string.end(), arg);
				break;
			case 'a':
				string = std::string(va_arg(*arg, char *));
				finish_expand(string.begin(), string.end(), arg);
				break;
			case 'n':
				(void)(va_arg(*arg,void(Request::*)(void)));
				break;
			case 'r' :
				i = va_arg(*arg, int);
				i = va_arg(*arg, int);
				break;
			case 'R' :
				str = va_arg(*arg, char *);
				break;
			case 'c' :
				i = va_arg(*arg, int);
				break;
			case 'C' :
				str = va_arg(*arg, char *);
				break;
			default:
				throw std::invalid_argument("error _and wrong arg");
				break;
		}
		start++;
	}
	(void)i;
	(void)str;
	(void)string;
}

void Request::expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg)
{
	int type;
	int n;
	int m;
	switch (*fct_it_tag)
	{
		case 's':
			n = va_arg(*arg, int);
			m = va_arg(*arg, int);
			n_star_m(n, m, va_arg(*arg,void(Request::*)(void)));
			break;
		case 'S':
			type = va_arg(*arg, int);
			n = va_arg(*arg, int);
			m = va_arg(*arg, int);
			switch (type)
			{
				case AND:
					n_star_m_and(n,m, arg);
					break;
				case OR:
					n_star_m_or(n,m, arg);
					break;
				default:
					throw std::invalid_argument("error _and wrong arg S");
					break;
			}
			break;
		case 'o':
			_or(std::string(va_arg(*arg, char *)) , arg);
			break;
		case 'a':
			_and(std::string(va_arg(*arg, char *)) , arg);
			break;
		case 'n':
			 (this->*va_arg(*arg,void(Request::*)(void)))();
			break;
		case 'r' :
			_range(va_arg(*arg, int),va_arg(*arg, int));
			break;
		case 'R' :
			_is_str(std::string(va_arg(*arg, char *)));
			break;
		case 'c' :
			_is_char(va_arg(*arg, int));
			break;
		case 'C' :
			_is_charset(std::string(va_arg(*arg, char *)));
			break;
		default:
			throw std::invalid_argument("error _and wrong arg");
			break;
	}
}

void Request::http_message()
{
	try
	{
		_and("nSns", &Request::start_line, AND, STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::header_field, &Request::CRLF, &Request::CRLF, STAR_NO_MIN, 1, &Request::message_body);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::start_line()
{
	try
	{
		request_line();
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::request_line()
{
	try
	{
		_and("nnnnnn", &Request::method, &Request::SP, &Request::request_target, &Request::SP, &Request::http_version, &Request::CRLF);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::method()
{
	try
	{
		_or("nnn", &Request::_get, &Request::_post, &Request::_delete);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::_get()
{
	_is_str("GET");
	_method = GET;
}
void Request::_post()
{
	_is_str("POST");
	_method = POST;
}
void Request::_delete()
{
	_is_str("DELETE");
	_method = DELETE;
}

void Request::request_target()
{
	size_t old_head = _head;

	try
	{
		_or("nnnn", &Request::origin_form, &Request::absolute_form, &Request::authority_form, &Request::asterisk_form);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_var_map["REQUEST_TARGET"] = std::string(_raw_str.begin()+ old_head + 1,_raw_str.begin() + _head);
}

void Request::origin_form()
{
	try
	{
		_and("nS", &Request::absolute_path, AND, STAR_NO_MIN, 1, "cn", '?', &Request::query);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::absolute_path()
{
	size_t old_head = _head;
	try
	{
		n_star_m_and(1, STAR_NO_MAX, "cn", '/', &Request::segment);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	CATCH_VAR("ABSOLUTE_PATH");
}

void Request::segment()
{
	try
	{
		n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::PCHAR);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}
void Request::query()
{
	size_t old_head = _head;
	try
	{
		n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "ncc", &Request::PCHAR, '/', '?');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	CATCH_VAR("QUERY_STRING");
}

void Request::absolute_form()
{
	try
	{
		absolute_URI();
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::asterisk_form()
{
	try
	{
		throw std::invalid_argument("asterisk form only used for OPTION method");
		_is_char('*');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::absolute_URI()
{
	try
	{
		_and("ncnS", &Request::scheme, ':', &Request::hier_part,  AND, STAR_NO_MIN, 1, "cn", '?', &Request::query);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::hier_part()
{
	try
	{
		_and("RS", "//", OR, STAR_NO_MIN, 1 , "nnnn", &Request::authority, &Request::path_abempty, &Request::path_absolute, &Request::path_rootless);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::authority()
{
	size_t old_head = _head;

	try
	{
		_and("SnS", AND, STAR_NO_MIN, 1, "nc", &Request::userinfo, '@', &Request::host, AND, STAR_NO_MIN, 1, "cn", ':', &Request::port);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	CATCH_VAR("AUTHORITY");
}

void Request::userinfo()
{
	try
	{
		n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nnnc", &Request::unreserved, &Request::pct_encoded, &Request::sub_delims, ':');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::host()
{
	try
	{
		_or("nnn", &Request::IP_literal, &Request::IPv4address, &Request::reg_name);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::IP_literal()
{
	try
	{
		_and("cSc", '[', &Request::IPv6address, &Request::IPvFuture, ']');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::IPv6address()
{
	try
	{
		_or("aaaaaaa",
			"Sn",	   AND, 6, 6, "nc", &Request::h16, ':', &Request::ls32,
			"RSn",	  "::", AND, 5, 5, "nc", &Request::h16, ':', &Request::ls32,
			"sRSn",	 STAR_NO_MIN, 1, &Request::h16, "::", AND, 4, 4, "nc", &Request::h16, ':', &Request::ls32,
			"SRSn",	 AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 1, "nc", &Request::h16, ':', &Request::h16, "::", AND, 3, 3, "nc", &Request::h16, ':', &Request::ls32,
			"SRSn",	 AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 2, "nc", &Request::h16, ':', &Request::h16, "::", AND, 2, 2, "nc", &Request::h16, ':', &Request::ls32,
			"SRncn",	AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 3, "nc", &Request::h16, ':', &Request::h16, "::", &Request::h16, ':', &Request::ls32,
			"SRn",	  AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 4, "nc", &Request::h16, ':', &Request::h16, "::", &Request::ls32,
			"SRn",	  AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 5, "nc", &Request::h16, ':', &Request::h16, "::", &Request::h16,
			"SR",	   AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 5, "nc", &Request::h16, ':', &Request::h16, "::");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::h16()
{
	try
	{
		n_star_m(1, 4, &Request::HEXDIG);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::ls32()
{
	try
	{
		_or("an", "ncn", &Request::h16, ':', &Request::h16, &Request::IPv4address);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::IPvFuture()
{
	try
	{
		_and("cscS", 'v', 1, STAR_NO_MAX, &Request::HEXDIG, '.', OR, 1, STAR_NO_MAX, "nnc", &Request::unreserved, &Request::sub_delims, ':');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::IPv4address()
{
	try
	{
		_and("ncncncn", &Request::dec_octet, '.', &Request::dec_octet, '.', &Request::dec_octet, '.', &Request::dec_octet);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::dec_octet()
{
	try
	{
		_or("aaaan", "Rr", "25", '0', '5',  "crn", '2', '0', '4', &Request::DIGIT, "cnn", '1', &Request::DIGIT, &Request::DIGIT, "rn", '1', '9', &Request::DIGIT, &Request::DIGIT);
		// _or("naaaa", &Request::DIGIT, "rn", '1', '9', &Request::DIGIT,"cnn", '1', &Request::DIGIT, &Request::DIGIT, "crn", '2', '0', '4', &Request::DIGIT, "Rr", "25", '0', '5');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::reg_name()
{
	try
	{
		n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nnn", &Request::unreserved, &Request::pct_encoded, &Request::sub_delims);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::port()
{
	try
	{
		n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::DIGIT);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::path_abempty()
{
	try
	{
		n_star_m_and(STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::path_absolute()
{
	try
	{
		_and("cS", "/", AND, STAR_NO_MIN, 1, "nS", &Request::segment_nz, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Request::segment_nz()
{
	try
	{
		n_star_m(1, STAR_NO_MAX, &Request::PCHAR);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::path_rootless()
{
	try
	{
		_and("nS", &Request::segment_nz, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::http_version()
{
	size_t old_head = _head;
	try
	{
		_and("ncscs", &Request::http_name, '/', 1, STAR_NO_MAX, &Request::DIGIT, '.', 1, STAR_NO_MAX, &Request::DIGIT);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	_head += 5;
	old_head = _head;
	_and("scs", 1, STAR_NO_MAX, &Request::DIGIT, '.', 1, STAR_NO_MAX, &Request::DIGIT);
	CATCH_VAR("HTTP_VERSION");
}

void Request::http_name()
{
	try
	{
		_is_str("HTTP");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::authority_form()
{
	try
	{
		throw std::invalid_argument("authority form only used for CONNECT method");
		authority();
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::header_field()
{
	size_t old_head = _head;
	try
	{
		_and("ncnnn", &Request::field_name, ':', &Request::OWS, &Request::field_value, &Request::OWS);
		catch_var_header_field(old_head);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::catch_var_header_field(size_t old_head)
{
	std::string var_name;

	_head = old_head;
	field_name();
	var_name = std::string(_raw_str.begin() + old_head, _raw_str.begin() + _head);
	_head += 2;
	old_head = _head;
	field_value();
	CATCH_HEADER_VAR(var_name);
	OWS();
}

void Request::field_name()
{
	try
	{
		token();
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::field_value()
{
	try
	{
		n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::field_content, &Request::obs_fold);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::field_content()
{
	try
	{
		_and("nS", &Request::field_vchar, AND, STAR_NO_MIN, 1, "Sn", OR, 1, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB, &Request::field_vchar);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::field_vchar()
{
	try
	{
		_or("nn", &Request::VCHAR, &Request::obs_text);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::VCHAR()
{
	try
	{
		_range(0x21, 0x7e);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::obs_text()
{
	try
	{
		_range(0x80, 0xFF);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::obs_fold()
{
	try
	{
		_and("nS", &Request::CRLF, AND, 1, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB);
	}
	catch(const std::exception& e)
	{
		throw std::invalid_argument(std::string(__FUNCTION__) + "/"+ std::string(e.what()));
	}
}

void Request::message_body()
{
	_head_msg_body = _head;
	// throw std::invalid_argument("message_body");
	// try
	// {
	// 	n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::OCTET);
	// }
	// catch(const std::exception& e)
	// {
	// 	throw EXECP;
	// }
}

void		Request::set_raw_request(const std::string& raw_request)
{
	_raw_str = raw_request;
	_head = 0;
	_method = NO_METHOD;
	_var_map.clear();
	_error_msg = "";
	_is_valid = false;
	try
	{

		http_message();
		_is_valid = true;
		std::cout << "http_message OK" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << "error req/" << e.what() << '\n';
		_error_msg = e.what();
		std::cout << "http_message KO" << std::endl;
		std::cout << std::endl << "'"<< &_raw_str[_head] << "'" << std::endl;
	}
}

int Request::get_method() const
{
	return (_method);
}

std::string const Request::get_raw_request() const
{
	return (_raw_str);
}

std::string const Request::get_connection() const
{
	std::pair<bool, std::string> ret = get_header_var_by_name("Connection");

	return  ((ret.first) ? ret.second : "");
}

std::string const Request::get_authority() const
{
	std::pair<bool, std::string> ret = get_header_var_by_name("Authority");

	return  ((ret.first) ? ret.second : "");
}

std::string const Request::get_host() const
{
	std::pair<bool, std::string> ret = get_header_var_by_name("Host");

	return  ((ret.first) ? ret.second : "");
}

std::string const Request::get_request_target() const
{
	std::pair<bool, std::string> ret = get_var_by_name("REQUEST_TARGET");

	return  ((ret.first) ? ret.second : "");
}

std::string const Request::get_message_body() const
{
	return (std::string(_raw_str.begin() + _head_msg_body, _raw_str.end()));
}

std::pair<bool, std::string> Request::get_var_by_name(const std::string &name) const
{
	try
	{
		std::string str = _var_map.at(name);
		return (std::pair<bool, std::string>(true, str));
	}
	catch(const std::exception& e)
	{
		return (std::pair<bool, std::string>(false, ""));
	}
}

std::pair<bool, std::string> Request::get_header_var_by_name(const std::string &name) const
{
	try
	{
		std::string str = _header_var_map.at(name);
		return (std::pair<bool, std::string>(true, str));
	}
	catch(const std::exception& e)
	{
		return (std::pair<bool, std::string>(false, ""));
	}
}

std::map<std::string, std::string> Request::get_var_map() const
{
	return (_var_map);
}

std::map<std::string, std::string> Request::get_header_var_map() const
{
	return (_header_var_map);
}

void Request::status_line()
{
	try
	{
		_and("nnnn", &Request::http_version, &Request::SP, &Request::status_code_phrase,  &Request::CRLF);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Request::status_code_phrase()
{
	size_t old_head = _head;
	try
	{
		_or("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR",
			"100 Continue",
			"101 Switching Protocols",
			"200 OK",
			"201 Created",
			"202 Accepted",
			"203 Non-Authoritative Information",
			"204 No Content",
			"205 Reset Content",
			"206 Partial Content",
			"300 Multiple Choices",
			"301 Moved Permanently",
			"302 Found",
			"303 See Other",
			"304 Not Modified",
			"305 Use Proxy",
			"307 Temporary Redirect",
			"400 Bad Request",
			"401 Unauthorized",
			"402 Payment Required",
			"403 Forbidden",
			"404 Not Found",
			"405 Method NotAllowed",
			"406 Not Acceptable",
			"407 Proxy AuthenticationRequired",
			"408 Request Time-out",
			"409 Conflict",
			"410 Gone",
			"411 Length Required",
			"412 Precondition Failed",
			"413 Request EntityToo Large",
			"414 Request-URI Too Large",
			"415 Unsupported MediaType",
			"416 Requested rangenot satisfiable",
			"417 Expectation Failed",
			"500 Internal ServerError",
			"501 Not Implemented",
			"502 Bad Gateway",
			"503 Service Unavailable",
			"504 Gateway Time-out",
			"505 HTTP Version not supported");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	CATCH_VAR("STATUS_CODE");
}
