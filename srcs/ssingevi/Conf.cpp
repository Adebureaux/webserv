#include "Conf.hpp"

Conf::Conf(const std::string& raw_Conf)
: Parser(raw_Conf), _current_location(), _current_server_block(), _serv_vector(), _current_error_code()
{
	try
	{
		_remove_comment();
		conf();
		// test_validity
		// _is_valid = true;
	}
	catch(const std::exception& e)
	{
		// _error_msg = e.what();
	}
}

Conf::Conf(const Conf &cpy)
: Parser::Parser()
{
	*this = cpy;
}

Conf &Conf::operator=(const Conf &src)
{
	_raw_str = src._raw_str;
	_head = src._head;

	return (*this);
}

Conf::~Conf(){}

void Conf::n_star_m(int n, int m, void(Conf::*fct)(void))
{
	n_star_m_or(n, m, "n", fct);
}

void Conf::finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg)
{
	int i;
	char *str;
	std::string string;
	void(Conf::*pf)(void);

	while ( start != end)
	{
		switch (*start)
		{
			case 's':
				i = va_arg(*arg, int);
				i = va_arg(*arg, int);
				(void)(va_arg(*arg,void(Conf::*)(void)));
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
				(void)(va_arg(*arg,void(Conf::*)(void)));
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

void Conf::expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg)
{
	int type;
	int n;
	int m;
	switch (*fct_it_tag)
	{
		case 's':
			n = va_arg(*arg, int);
			m = va_arg(*arg, int);
			n_star_m(n, m, va_arg(*arg,void(Conf::*)(void)));
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
			 (this->*va_arg(*arg,void(Conf::*)(void)))();
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


void Conf::OB(void)
{
	try
	{
		_is_char('{');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::CB(void)
{
	try
	{
		_is_char('}');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::end_of_line(void)
{
	try
	{
		_and("nn", &Conf::OWS, &Conf::LF);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}


void Conf::_remove_comment(void)
{
	while (1)
	{
		size_t	pos_diez = _raw_str.find("#", 0);
		if (pos_diez == std::string::npos)
			break ;
		size_t	pos_nl = _raw_str.find("\n", pos_diez);
		_raw_str.replace(pos_diez, pos_nl - pos_diez, "");
	}
}

// conf = 1*(OWS 'server' ':' OWS *1LF OWS OB end_of_line 1*server_block OWS CB end_of_line)
// nCcns

void Conf::conf(void)
{
	try
	{
		n_star_m_and(1, STAR_NO_MAX, "nRcnsnnnsnnn", &Conf::OWS, "server", ':', &Conf::OWS, STAR_NO_MIN, 1, &Conf::LF, &Conf::OWS, &Conf::OB, &Conf::end_of_line, 1, STAR_NO_MAX, &Conf::server_block, &Conf::OWS, &Conf::CB, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	
}

void Conf::server_block(void)
{
	static bool main = true;
	try
	{
		_and("nSn", &Conf::OWS, OR, STAR_NO_MIN, STAR_NO_MAX, "nnnnnn", &Conf::location_block ,&Conf::server_name ,&Conf::listen ,&Conf::root , &Conf::error_block, &Conf::body_size, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_server_block.main = main;
	main = false;
	_serv_vector.push_back(_current_server_block);
	_current_server_block = Server_block();
}

void Conf::location_block(void)
{
	try
	{
		_and("Rcnsnnnnnn", "location" ,':' , &Conf::OWS, STAR_NO_MIN, 1, &Conf::LF, &Conf::OB, &Conf::end_of_line, &Conf::location_var, &Conf::OWS, &Conf::CB, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_server_block.locations.push_back(_current_location);
	_current_location = Location();
}

// void Conf::path_location(void)
// {
// 	size_t old_head = _head;
// 	try
// 	{
// 		path();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		throw EXECP;
// 	}
// 	_current_location.root = std::string(_raw_str.begin()+ old_head,_raw_str.begin() + _head);
// }

// location_var = *(OWS ( method | autoindex | index | upload | cgi | root | redirect) end_of_line)

void Conf::location_var(void)
{
	try
	{
		n_star_m_and(STAR_NO_MIN, STAR_NO_MAX, "non", &Conf::OWS, "nnnnnnn", &Conf::method, &Conf::autoindex, &Conf::index, &Conf::upload, &Conf::cgi, &Conf::redirect, &Conf::root, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

// method = 'method' ':' OWS catch_method OWS *2 ( OWS ',' OWS catch_method) end_of_line

void Conf::method(void)
{
	try
	{
		_and("RcnnnSn", "method", ':', &Conf::OWS , &Conf::catch_method, &Conf::OWS, AND, STAR_NO_MIN, 2, "ncnn", &Conf::OWS, ',', &Conf::OWS, &Conf::catch_method, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}	
}

void Conf::catch_method(void)
{
	size_t old_head = _head;
	try
	{
		_or("RRR",  "GET", "POST" ,"DELETE");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	try
	{
		try
		{
			_is_str("GET");
			_current_location.get_method = true;
		}
		catch(const std::exception& e)
		{
			_is_str("POST");
			_current_location.post_method = true;
		}
	}
	catch (const std::exception& e)
	{
		_is_str("DELETE");
		_current_location.delete_method = true;
	}
}

void Conf::autoindex(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnon", "autoindex", ':', &Conf::OWS, "CC", "on", "off", &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	_is_str("autoindex:");
	OWS();
	try
	{
		_is_str("on");
		_current_location.autoindex = true;
	}
	catch(const std::exception& e)
	{
		_is_str("off");
		_current_location.autoindex = false;
	}
	end_of_line();
}

// index = 'index' ':' OWS path end_of_line

void Conf::index(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnnn", "index", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	_is_str("index:");
	OWS();
	old_head = _head;
	path();
	_current_location.default_file = std::string(_raw_str.begin() + old_head, _raw_str.begin() + _head);
	end_of_line();
}

// upload = 'upload' ':' OWS upload_value end_of_line

void Conf::upload(void)
{
	try
	{
		_and("Ccnon", "upload", ':', &Conf::OWS, "aC", "Cnn", "on", &Conf::OWS, &Conf::path, "off", &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

// upload_value = ('on' OWS path) | 'off')

void Conf::upload_value(void)
{
	try
	{
		_is_str("on");
		OWS();
		size_t old_head = _head;
		path();
		_current_location.upload = std::pair<bool, std::string>(true, std::string(_raw_str.begin() + old_head,_raw_str.begin() + _head));

	}
	catch(const std::exception& e)
	{
		try
		{
			_is_str("off");
			_current_location.upload = std::pair<bool, std::string>(true, "");
		}
		catch(const std::exception& e)
		{
			throw EXECP;
		}
	}
	
}

// cgi = 'cgi' ':' OWS path end_of_line

void Conf::cgi(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnnn", "cgi", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	_is_str("cgi:");
	OWS();
	old_head = _head;
	path();
	_current_location.CGI = std::string(_raw_str.begin()+ old_head,_raw_str.begin() + _head);
	end_of_line();
}

// server_name = 'server_name' ':' OWS server_name_value OWS end_of_line

void Conf::server_name(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnnnn", "server_name", ':', &Conf::OWS, &Conf::server_name_value, &Conf::OWS, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::server_name_value(void)
{
	size_t old_head = _head;
	try
	{
		n_star_m(1, STAR_NO_MAX, &Conf::unreserved);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_server_block.server_names = std::string(_raw_str.begin() + old_head,_raw_str.begin() + _head);
}
// listen = 'listen' ':' OWS port end_of_line

void Conf::listen(void)
{
	try
	{
		_and("Rcnnn", "listen", ':', &Conf::OWS, &Conf::port, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::port(void)
{
	size_t old_head = _head;
	try
	{
		n_star_m(1, STAR_NO_MAX, &Conf::DIGIT);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_server_block.port = atoi(std::string(_raw_str.begin() + old_head,_raw_str.begin() + _head).c_str());
}

// root = 'root' ':' OWS path end_of_line

void Conf::root(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnnn", "root", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	_is_str("root:");
	OWS();
	old_head = _head;
	path();
	_current_location.root = std::string(_raw_str.begin()+ old_head ,_raw_str.begin() + _head);
	end_of_line();
}

// error_block = 'error' ':' OWS *1LF OB end_of_line 1*(OWS error_code ':' OWS path end_of_line) OWS CB end_of_line

void Conf::error_block(void)
{
	try
	{
		_and("Rcnsnnsnnn", "error", ':', &Conf::OWS, STAR_NO_MIN, 1, &Conf::LF, &Conf::OB, &Conf::end_of_line, 1, STAR_NO_MAX, Conf::error_var, &Conf::OWS, &Conf::CB, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	
}

// error_var = OWS error_code ':' OWS path end_of_line

void Conf::error_var(void)
{
	size_t old_head = _head;	
	try
	{
		_and("nncnnn", &Conf::OWS, &Conf::error_code, ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		_current_error_code = 0;
		throw EXECP;
	}
	_head = old_head;
	OWS();
	error_code();
	_is_char(':');
	OWS();
	old_head = _head;
	path();
	_current_server_block.error_pages[_current_error_code] = std::string(_raw_str.begin()+ old_head ,_raw_str.begin() + _head);
	_current_error_code = 0;
	end_of_line();
}

void Conf::error_code(void)
{
	size_t old_head = _head;
	try
	{
		_or("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR",
		"100", "101", "200", "201", "202", "203", "204", "205", "206", "300",
		"301", "302", "303", "304", "305", "307", "400", "401", "402", "403",
		"404", "405", "406", "407", "408", "409", "410", "411", "412", "413",
		"414", "415", "416", "417", "500", "501", "502", "503", "504", "505");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_error_code = atoi(std::string(_raw_str.begin() + old_head, _raw_str.begin() + _head).c_str());
}

// host = 'host' ':' OWS IPv4address end_of_line

void Conf::host(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnnn", "host", ':', &Conf::OWS, &Conf::IPv4address, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::IPv4address()
{
	size_t old_head = _head;
	try
	{
		_and("ncncncn", &Conf::dec_octet, '.', &Conf::dec_octet, '.', &Conf::dec_octet, '.', &Conf::dec_octet);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_server_block.address = std::string(_raw_str.begin() + old_head,_raw_str.begin() + _head);
}

void Conf::dec_octet()
{
	try
	{
		_or("naaaa", &Conf::DIGIT, "rn", '1', '9', &Conf::DIGIT,"cnn", '1', &Conf::DIGIT, &Conf::DIGIT, "crn", '2', '0', '4', &Conf::DIGIT, "Rr", "25", '0', '5');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::path(void)
{
	try
	{
		n_star_m_and(1, STAR_NO_MAX, "cs", '/', 1, STAR_NO_MAX, &Conf::unreserved);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::redirect(void)
{
	size_t old_head = _head;
	try
	{
		_and("Rcnnn", "redirect", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_head = old_head;
	_is_str("redirect:");
	OWS();
	old_head = _head;
	path();
	_current_location.redirect = std::string(_raw_str.begin()+ old_head,_raw_str.begin() + _head);
	end_of_line();
}

void Conf::body_size(void)
{
	try
	{
		_and("Rcnnn", "body_size", ':', &Conf::OWS, &Conf::body_size_value, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	
}

void Conf::body_size_value(void)
{
	size_t old_head = _head;
	try
	{
		n_star_m(1, STAR_NO_MAX, &Conf::DIGIT);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	_current_server_block.body_size = atoi(std::string(_raw_str.begin() + old_head,_raw_str.begin() + _head).c_str());
}