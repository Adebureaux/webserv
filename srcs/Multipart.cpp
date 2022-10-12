#include "../includes/Multipart.hpp"

Multipart::Multipart(const std::string& raw_multipart, const std::string& boundary)
: Parser(raw_multipart), _is_finish(false), _is_valid(false), _boundary(boundary), _current_header_field(), _files(), _error_msg()
{
	try
	{
		// std::cout << __FUNCTION__ << raw_multipart << std::endl;
		start_parsing();
		std::cout << __FUNCTION__  << "	is_finish = " << _is_finish << std::endl << raw_multipart << std::endl;
		_is_valid = true;
	}
	catch(const std::exception& e)
	{
		_error_msg = e.what();
		_files.clear();
	}

}

Multipart::Multipart(const std::string& boundary)
: Parser(), _is_finish(false), _is_valid(false), _boundary(boundary), _current_header_field(), _files(), _error_msg()
{}

Multipart::Multipart(const Multipart &cpy)
{
	*this = cpy;
}

Multipart &Multipart::operator=(const Multipart &src)
{
	_is_finish = src._is_finish;
	_is_valid = src._is_valid;
	_boundary = src._boundary;
	_current_header_field = src._current_header_field;
	_files = src._files;
	_error_msg = src._error_msg;
	return (*this);
}

Multipart::~Multipart(){}

void Multipart::n_star_m(int n, int m, void(Multipart::*fct)(void))
{
	n_star_m_or(n, m, "n", fct);
}

void Multipart::finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg)
{
	int i;
	char *str;
	std::string string;
	void(Multipart::*pf)(void);

	while ( start != end)
	{
		switch (*start)
		{
			case 's':
				i = va_arg(*arg, int);
				i = va_arg(*arg, int);
				(void)(va_arg(*arg,void(Multipart::*)(void)));
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
				(void)(va_arg(*arg,void(Multipart::*)(void)));
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

void Multipart::expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg)
{
	int type;
	int n;
	int m;
	switch (*fct_it_tag)
	{
		case 's':
			n = va_arg(*arg, int);
			m = va_arg(*arg, int);
			n_star_m(n, m, va_arg(*arg,void(Multipart::*)(void)));
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
			 (this->*va_arg(*arg,void(Multipart::*)(void)))();
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

void Multipart::start_parsing(void)
{
	try
	{
		_and("nnSnn", &Multipart::is_boundary, &Multipart::CRLF, AND, STAR_NO_MIN, STAR_NO_MAX, "nn", &Multipart::header_field, &Multipart::CRLF, &Multipart::CRLF, &Multipart::message_body);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}


void Multipart::is_boundary_end(void)
{
	// size_t old_head = _head;
	try
	{
		_and("nR", &Multipart::is_boundary, "--");
		std::cout << __FUNCTION__ << std::endl;
		
		_is_finish = true;
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Multipart::is_boundary(void)
{
	try
	{
		_is_str(_boundary);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Multipart::header_field()
{
	size_t old_head = _head;
	try
	{
		_and("ncnnn", &Multipart::field_name, ':', &Multipart::OWS, &Multipart::field_value, &Multipart::OWS);
		std::cout << __FUNCTION__ << std::endl;
		_current_header_field +=  std::string(_raw_str.begin()+ old_head,_raw_str.begin() + _head);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Multipart::field_name()
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

void Multipart::field_value()
{
	try
	{
		n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nn", &Multipart::field_content, &Multipart::obs_fold);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Multipart::field_content()
{
	try
	{
		_and("nS", &Multipart::field_vchar, AND, STAR_NO_MIN, 1, "Sn", OR, 1, STAR_NO_MAX, "nn", &Multipart::SP, &Multipart::HTAB, &Multipart::field_vchar);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Multipart::field_vchar()
{
	try
	{
		_or("nn", &Multipart::VCHAR, &Multipart::obs_text);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Multipart::VCHAR()
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

void Multipart::obs_text()
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

void Multipart::obs_fold()
{
	try
	{
		_and("nS", &Multipart::CRLF, AND, 1, STAR_NO_MAX, "nn", &Multipart::SP, &Multipart::HTAB);
	}
	catch(const std::exception& e)
	{
		throw std::invalid_argument(std::string(__FUNCTION__) + "/"+ std::string(e.what()));
	}
}

void Multipart::message_body(void)
{
	std::cout << __FUNCTION__ << std::endl;
	size_t new_head = _raw_str.find(_boundary, _head);
	if (new_head == std::string::npos)
		throw EXECP_("request not valid");
	if (_files.find(_current_header_field) == _files.end())
		_files[_current_header_field] = File_Multipart(_current_header_field);
	_files[_current_header_field] += std::string(_raw_str.begin()+ _head,_raw_str.begin() + new_head - 2);
	_head = new_head;
	_current_header_field.clear();
	_or("nn", &Multipart::is_boundary_end, &Multipart::is_boundary);
	CRLF();
	if (_head == _raw_str.size())
		return;
	if( _is_finish)
		throw EXECP_("request not valid");
	else
	{
		
		_head = new_head;
		start_parsing();
	}
}

void Multipart::set_new_multipart(std::string const &new_str)
{
	_raw_str = new_str;
	_head = 0;
	try
	{
		if (_is_finish)
			throw EXECP_("POST already finish");
		_is_valid = false;
		start_parsing();
		_is_valid = true;
	}
	catch(const std::exception& e)
	{
		_error_msg = e.what();
		_files.clear();
	}

}

void Multipart::set_boundary(std::string const &boundary)
{
	if (_boundary.empty())
		_boundary = boundary;
	else
		throw EXECP_("boundary already set");
}

std::map<std::string, File_Multipart>		Multipart::get_files()
{
	if (!_is_finish)
			throw EXECP_("POST not finish");
	return (_files);
}


File_Multipart::File_Multipart(const std::string& var)
{
	size_t pos = var.find("filename=");
	if (pos == std::string::npos)
		throw EXECP_("request not valid");
	pos += 9;
	while (var[pos] != '\"')
		pos++;
	pos++;
	for (size_t i = pos; i < var.size(); i++)
	{
		if (var[i] == '\"')
			_filename = std::string(var.begin()+ pos,var.begin() + i);
	}
	pos = var.find("Content-Type:");
	if (pos == std::string::npos)
		throw EXECP_("request not valid");
	pos += 13;
	while (var[pos] == '\t' || var[pos] == ' ')
		pos++;
	_content_type = std::string(var.begin()+ pos,var.end());
}

File_Multipart::File_Multipart()
{}

File_Multipart::File_Multipart(const File_Multipart &cpy)
{
	*this = cpy;
}
File_Multipart &File_Multipart::operator=(const File_Multipart &src)
{
	_filename = src._filename;
	_content_type = src._content_type;
	_file = src._file;
	return (*this);
}

File_Multipart::~File_Multipart()
{}

const std::string &File_Multipart::get_filename(void) const
{
	return (_filename);
}

const std::string &File_Multipart::get_content_type(void) const
{
	return(_content_type);
}

const std::string &File_Multipart::get_file(void) const
{
	return(_file);
}

File_Multipart	&File_Multipart::operator+=(const std::string& src)
{
	_file += src;
	return(*this);
}