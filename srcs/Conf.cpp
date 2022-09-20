#include "Conf.hpp"

Conf::Conf(const std::string& raw_Conf)
: Parser(raw_Conf)
{
	try
	{
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

// bool Conf::is_valid()
// {
// 	return(_is_valid);
// }

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
	try
	{
		_and("nSn", &Conf::OWS, OR, STAR_NO_MIN, STAR_NO_MAX, "nnnnn", &Conf::location_block ,&Conf::server_name ,&Conf::listen ,&Conf::root , &Conf::error_block, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::location_block(void)
{
	try
	{
		_and("Rcnnnsnnnnnn", "location" ,':' , &Conf::OWS, &Conf::path_location, &Conf::OWS, STAR_NO_MIN, 1, &Conf::LF, &Conf::OB, &Conf::end_of_line, &Conf::location_var, &Conf::OWS, &Conf::CB, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	
}

void Conf::path_location(void)
{
	try
	{
		path();
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
	
}

// location_var = *(OWS ( method | Autoindex | index | upload | cgi) end_of_line)

void Conf::location_var(void)
{
	try
	{
		n_star_m_and(STAR_NO_MIN, STAR_NO_MAX, "non", &Conf::OWS, "nnnnn", &Conf::method, &Conf::Autoindex, &Conf::index, &Conf::upload, &Conf::cgi, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

// method = 'method' ':' OWS ('GET' | 'POST' | 'DELETE') *2 ( OWS ',' OWS ('GET' | 'POST' | 'DELETE')) end_of_line

void Conf::method(void)
{
	try
	{
		_and("RcnoSn", "method", ':', &Conf::OWS , "RRR", "GET", "POST" ,"DELETE", AND, STAR_NO_MIN, 2, "ncno", OWS, ',', OWS, "RRR", "GET", "POST" ,"DELETE", end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}	
}

// Autoindex = 'Autoindex' ':' OWS ('on' | 'off') end_of_line

void Conf::Autoindex(void)
{
	try
	{
		_and("Rcnon", "Autoindex", ':', &Conf::OWS, "CC", "on", "off", &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}	
}

// index = 'index' ':' OWS path end_of_line

void Conf::index(void)
{
	try
	{
		_and("Rcnnn", "index", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

// upload = 'upload' ':' OWS ('on' OWS path) | 'off') end_of_line

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

// cgi = 'cgi' ':' OWS path end_of_line

void Conf::cgi(void)
{
	try
	{
		_and("Rcnnn", "cgi", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

// server_name = 'server_name' ':' OWS 1*unreserved OWS end_of_line

void Conf::server_name(void)
{
	try
	{
		_and("Rcnsn", "server_name", ':', &Conf::OWS, 1, STAR_NO_MAX, &Conf::unreserved, &Conf::OWS, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
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
	try
	{
		n_star_m(1, STAR_NO_MAX, &Conf::DIGIT);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

// root = 'root' ':' OWS path end_of_line

void Conf::root(void)
{
	try
	{
		_and("Rcnnn", "root", ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
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
	try
	{
		_and("nncnnn", &Conf::OWS, &Conf::error_code, ':', &Conf::OWS, &Conf::path, &Conf::end_of_line);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Conf::error_code(void)
{
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
}

// host = 'host' ':' OWS IPv4address end_of_line

void Conf::host(void)
{
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
	try
	{
		_and("ncncncn", &Conf::dec_octet, '.', &Conf::dec_octet, '.', &Conf::dec_octet, '.', &Conf::dec_octet);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
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
