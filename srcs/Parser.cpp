#include "../includes/Parser.hpp"

Parser::Parser() : _head(0), _raw_str()
{

}
Parser::Parser(const std::string& str) : _head(0), _raw_str(str)
{

}
Parser::Parser(const Parser &cpy)
{
	*this =cpy;
}
Parser &Parser::operator=(const Parser &src)
{
	_head = src._head;
	_raw_str = src._raw_str;
	return(*this);
}

Parser::~Parser()
{

}

void Parser::CR()
{
	_is_char('\r');
}

void Parser::CRLF()
{
	try
	{
		_and("nn", &Parser::CR, &Parser::LF);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Parser::LF()
{
	_is_char('\n');
}

void Parser::HTAB()
{
	_is_char('\t');
}

void Parser::SP()
{
	_is_char(' ');
}

void Parser::DIGIT()
{
	_range('0', '9');
}

void Parser::HEXDIG()
{
	_or("nr", &Parser::DIGIT, 'A', 'F');
}

void Parser::BIT()
{
	_range('0', '1');
}

void Parser::UPALPHA()
{
	_range('A', 'Z');
}

void Parser::LOALPHA()
{
	_range('a', 'z');
}

void Parser::ALPHA()
{
	_or("nn", &Parser::UPALPHA, &Parser::LOALPHA);
}

void Parser::OCTET()
{
	_range(0x00, 0xFF);
}

void Parser::_is_str(std::string const &str)
{
	if(str.compare(0, str.length(), _raw_str, _head, str.length()))
		throw std::invalid_argument(str + " was expected");
	_head += str.length();
}

void Parser::_range(char start, char end)
{
	if (_raw_str[_head] < start || _raw_str[_head] > end)
		throw std::invalid_argument("char between '" + std::string(&start, 1) + "' and '" + std::string(&end, 1) + "' was expected");
	else if (_head >= _raw_str.size())
		throw std::out_of_range(" out of range ");
	_head++;
}

void Parser::_is_charset(std::string const &charset)
{
	std::string::const_iterator it = charset.begin();

	for (; it != charset.end(); ++it)
	{
		if (*it == _raw_str[_head])
		{
			if (_head >= _raw_str.size())
				throw std::out_of_range(" out of range ");
			_head++;
			return;
		}
	}
	throw std::invalid_argument(charset + " was expected");
}

void Parser::_is_char(char c)
{
	// std::cout << "_is_char " << '\''<< c << '\''<< std::endl;
	if (_raw_str[_head] != c)
	{
		switch (c)
		{
		case '\r':
			throw std::invalid_argument("char '\\r' was expected");
			break;
		case '\n':
			throw std::invalid_argument("char '\\n' was expected");
			break;
		case '\t':
			throw std::invalid_argument("char '\\t' was expected");
			break;
		default:
			throw std::invalid_argument("char '" + std::string(&c, 1) + "' was expected");
			break;
		}
	}
	if (_head >= _raw_str.size())
				throw std::out_of_range(" out of range ");
	_head++;
}

void Parser::tchar()
{
	try
	{
		_or("nnC", &Parser::DIGIT, &Parser::ALPHA, "!#$%&\'*+-.^_\\`|~");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Parser::PCHAR()
{
	try
	{
		_or("nnncc", &Parser::unreserved, &Parser::pct_encoded, &Parser::sub_delims, ':', '@');
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Parser::unreserved()
{
	try
	{
		_or("Cnn", "-._~", &Parser::DIGIT, &Parser::ALPHA);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Parser::pct_encoded()
{
	try
	{
		_and("cnn", '%', &Parser::HEXDIG, &Parser::HEXDIG);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Parser::sub_delims()
{
	// std::cout << "sub_delims" << std::endl;
	try
	{
		_is_charset("!$&'()*+,;=");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}

}

void Parser::scheme()
{
	try
	{
		_and("nS", &Parser::ALPHA, OR, STAR_NO_MIN, STAR_NO_MAX, "nnC", &Parser::ALPHA, &Parser::DIGIT, "+-.");
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Parser::WSP(void)
{
	try
	{
		_or("nn", &Parser::SP, &Parser::HTAB);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Parser::OWS()
{
	try
	{
		n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Parser::WSP);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}

void Parser::token()
{
	try
	{
		n_star_m(1, STAR_NO_MAX, &Parser::tchar);
	}
	catch(const std::exception& e)
	{
		throw EXECP;
	}
}


void Parser::n_star_m_or(int n, int m, ...)
{
	va_list arg;

	va_start(arg, m);
	n_star_m_or(n, m, &arg);
	va_end(arg);
}

void Parser::n_star_m_or(int n, int m, va_list *arg)
{
	std::string fct_ptr_tag = va_arg(*arg, char *);
	int i;
	va_list ap;

	try
	{
		for (i = 0; i != m; i++)
		{
			ft_va_copy(&ap, arg);
			_or(fct_ptr_tag, &ap);
			va_end(ap);
		}
		finish_expand(fct_ptr_tag.begin(), fct_ptr_tag.end(), arg);
	}
	catch(const std::exception& e)
	{
		va_end(ap);
		finish_expand(fct_ptr_tag.begin(), fct_ptr_tag.end(), arg);
		if(i < n || (m != -1 && i > m))
			throw std::invalid_argument(e.what());
		else
			return;
	}
}

void Parser::n_star_m_and(int n, int m, ...)
{
	va_list arg;

	va_start(arg, m);
	n_star_m_and(n, m, &arg);
	va_end(arg);
}

void Parser::n_star_m_and(int n, int m, va_list *arg)
{
	std::string fct_ptr_tag = va_arg(*arg, char *);
	va_list ap;
	int i = 0;
	try
	{
		for (i = 0; i != m; i++)
		{
			ft_va_copy(&ap, arg);
			_and(fct_ptr_tag, &ap);
			va_end(ap);
		}
		finish_expand(fct_ptr_tag.begin(), fct_ptr_tag.end(), arg);
	}
	catch(const std::exception& e)
	{
		va_end(ap);
		finish_expand(fct_ptr_tag.begin(), fct_ptr_tag.end(), arg);
		if (i == n)
			return;
		else if(i < n || (m != -1 && i > m))
			throw std::invalid_argument(e.what());
		return;
	}

}

void Parser::_or(const std::string fct_ptr_tag, ...)
{
	va_list arg;

	va_start(arg, fct_ptr_tag);
	_or(fct_ptr_tag, &arg);
	va_end(arg);
}

void Parser::_and(const std::string fct_ptr_tag, ...)
{
	va_list arg;
	va_start(arg, fct_ptr_tag);
	_and(fct_ptr_tag, &arg);
	va_end(arg);
}


void Parser::ft_va_copy(va_list *dest, va_list *src)
{
	memcpy(dest, src, sizeof(va_list));
}

void Parser::_or(const std::string &fct_ptr_tag, va_list *arg)
{
	std::string::const_iterator start = fct_ptr_tag.begin();
	std::string::const_iterator end = fct_ptr_tag.end();
	static int i = -1;
	i++;
	size_t old_head = _head;
	while (start != end)
	{
		try
		{
			expand_va_arg(start, arg);
			start++;
			if (start != end)
				finish_expand(start, end, arg);
			return;
		}
		catch(const std::exception& e)
		{
			if(start +1 >= end)
			{
				_head = old_head;
				throw std::invalid_argument("error _or/" + std::string(e.what()));
			}
		}
		start++;
	}
}

void Parser::_and(const std::string &fct_ptr_tag, va_list *arg)
{
	std::string::const_iterator start = fct_ptr_tag.begin();
	std::string::const_iterator end = fct_ptr_tag.end();
	static int i = -1;
	size_t old_head = _head;
	i++;
	try
	{

		while (start != end)
		{
			expand_va_arg(start, arg);
			start++;
		}
	}
	catch(const std::exception& e)
	{
		_head = old_head;
		start++;
		if (start != end)
			finish_expand(start, end, arg);

		throw std::invalid_argument(e.what());
	}
}

void Parser::n_star_m(int n, int m, void(Parser::*fct)(void))
{
	n_star_m_or(n, m, "n", fct);
}