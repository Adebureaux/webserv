# include "Request.hpp"

Request::Request(const std::string& raw_request)
{

}
va_list *Request::ret_copy(va_list src)
{
    va_list dst;
    va_copy(dst,src);
    return &dst;
}

void Request::n_star_m_or(int n, int m, ...)
{
    va_list arg;

    va_start(arg, m);
    n_star_m_or(n, m, arg);
}

void Request::n_star_m_or(int n, int m, va_list arg)
{
    std::string fct_ptr_tag = va_arg(arg, char *);

    va_list *ap = ret_copy(arg);
    int i = 0;
    try
    {
        _or(fct_ptr_tag, arg);
        for (i = ((n > 0) ? 1 : 0); i != m; i++)
        {
            _or(fct_ptr_tag, *ret_copy(*ap));
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Request::n_star_m_and(int n, int m, ...)
{
    va_list arg;

    va_start(arg, m);
    n_star_m_and(n, m, arg);
}

void Request::n_star_m_and(int n, int m, va_list arg)
{
    std::string fct_ptr_tag = va_arg(arg, char *);

    va_list *ap = ret_copy(arg);
    int i = 0;

    _and(fct_ptr_tag, arg);
    for (i = ((n > 0) ? 1 : 0); i != m; i++)
    {
        _and(fct_ptr_tag, *ret_copy(*ap));
    }
}

void Request::_or(const std::string &fct_ptr_tag, ...)
{
    va_list arg;
    va_start(arg, fct_ptr_tag);
    _or(fct_ptr_tag, arg);        
}

void Request::_and(const std::string &fct_ptr_tag, ...)
{
    va_list arg;
    va_start(arg, fct_ptr_tag);
    _and(fct_ptr_tag, arg);        
}

void Request::expand_va_arg(std::string::const_iterator &fct_it_tag, va_list arg)
{
    switch (*fct_it_tag)
    {
        case 's':
            n_star_m(va_arg(arg, int), va_arg(arg, int), va_arg(arg, pf));
            break;
        case 'S':
            switch (va_arg(arg, int))
            {
            case AND:
                n_star_m_or(va_arg(arg, int),va_arg(arg, int), arg);
                break;
            case OR:
                n_star_m_or(va_arg(arg, int),va_arg(arg, int), arg);
            default:
                throw std::invalid_argument("error _and wrong arg S");
                break;
            }
            break;
        case 'o':
            _or(std::string(va_arg(arg, char *)) , arg);
            break;
        case 'a':
            _and(std::string(va_arg(arg, char *)) , arg);
            break;
        case 'n':
             (this->*va_arg(arg,void(Request::*)(void)))();
            break;
        case 'r' :
            _range(va_arg(arg, char),va_arg(arg, char));
            break;
        case 'R' :
            _is_str(va_arg(arg, char *));
            break;
        case 'c' :
            _is_char(va_arg(arg, char));
            break;
        case 'C' :
            _is_charset(std::string(va_arg(arg, char *)));
            break;
        default:
            throw std::invalid_argument("error _and wrong arg");
            break;
    }
}

void Request::_and(const std::string &fct_ptr_tag, va_list arg)
{
    std::string::const_iterator start = fct_ptr_tag.begin();
    std::string::const_iterator end = fct_ptr_tag.end();
    
    while (start != end)
    {
        expand_va_arg(start, arg);
        start++;
    }
}

void Request::_or(const std::string &fct_ptr_tag, va_list arg)
{
    std::string::const_iterator start = fct_ptr_tag.begin();
    std::string::const_iterator end = fct_ptr_tag.end();

    while (start != end)
    {
        try
        {
            expand_va_arg(start, arg);
            return;
        }
        catch(const std::exception& e)
        {
            if(start + 1 == end)
                throw std::invalid_argument("error _or wrong arg");
            (void)e;
        }
        start++;    
    }
}
        

void Request::http_message()
{
    _and("nSns", &start_line, AND, STAR_NO_MIN, STAR_NO_MAX, "nn", &header_field, &CRLF, &CRLF, STAR_NO_MIN, STAR_NO_MAX, &message_body);
}

void Request::start_line()
{
    request_line();
}

void Request::request_line()
{
    _and("nnnnnn", &method, &SP, &request_target, &SP, &http_version, &CRLF);
}

void Request::method()
{
    _or("nnn", &_get, &_post, &_delete);
}

void Request::_get()
{
    _is_str("GET");
}
void Request::_post()
{
    _is_str("POST");
}
void Request::_delete()
{
    _is_str("DELETE");
}

void Request::request_target()
{
    size_t old_head = _head;

    _or("nnnn", &origin_form, &absolute_form, &authority_form, &asterisk_form);
    _request_target = std::string(_raw_request, old_head, _head - old_head);
}

void Request::origin_form()
{
    _and("nS", &absolute_path, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '?', &query);
}

void Request::absolute_path()
{
    n_star_m_and(1, STAR_NO_MAX, "cn", '/', &segment);
}

void Request::segment()
{
    n_star_m(STAR_NO_MIN, STAR_NO_MAX, &PCHAR);
}

void Request::query()
{
    n_star_m_or(STAR_NO_MAX, STAR_NO_MIN, "ncc", &PCHAR, '/', '?');
}

void Request::LF()
{
    _is_char('\n');
}
    
void Request::HTAB()
{
    _is_char('\t');
}

void Request::SP()
{
    _is_char(' ');
}

void Request::DIGIT()
{
    _range('0', '9');
}

void Request::HEXDIG()
{
    _or("nr", &DIGIT, 'A', 'F');
}

void Request::BIT()
{
    _range('0', '1');
}

void Request::UPALPHA()
{
    _range('A', 'Z');
}

void Request::LOALPHA()
{
    _range('a', 'z');
}

void Request::ALPHA()
{
    _or("nn", &UPALPHA, &LOALPHA);
}

void Request::OCTET()
{
    _range(0x00, 0xFF);
}

void Request::segment()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, &PCHAR);
}

void Request::_is_str(std::string const &str)
{
    if(str.compare(_head, str.length(), _raw_request))
        throw std::invalid_argument(str + " was expected");
    _head += str.length();
}

void Request::_range(char start, char end)
{
    if (_raw_request[_head] < start || _raw_request[_head] > end)
        throw std::invalid_argument("char between '" + std::string(&start, 1) + "' and '" + std::string(&end, 1) + "' was expected");
    _head++;
}

void Request::_is_charset(std::string const &charset)
{
    std::string::const_iterator it = charset.begin();
    for (; it != charset.end(); ++it)
    {
        if (*it == _raw_request[_head])
        {
            _head++;
            return;
        }
    }
    throw std::invalid_argument(charset + " was expected");
}

void Request::_is_char(char c)
{
    if (_raw_request[_head] != c)
        throw std::invalid_argument("char '" + std::string(&c, 1) + "' was expected");
}

void Request::tchar()
{
    _or("nnC", &DIGIT, &ALPHA, "!#$%&\'*+-.^_\\`|~");
}

void Request::PCHAR()
{
    _or("nnncc", &unreserved, &pct_encoded, &sub_delims, ':', '@');
}

void Request::unreserved()
{
    _or("nnC", &ALPHA, &DIGIT, "-._~");
}

void Request::pct_encoded()
{
    _and("cnn", '%', &HEXDIG, &HEXDIG);
}

void Request::sub_delims()
{
    _is_charset("!$&'()*+,;=");
}

void Request::absolute_form()
{
    absolute_URI();
}

void Request::absolute_URI()
{
    _and("ncnS", &scheme, ':', &hier_part,  AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '?', &query);
}

void Request::scheme()
{
    _and("nS", &ALPHA, OR, STAR_NO_MIN, STAR_NO_MAX, "nnC", &ALPHA, &DIGIT, "+-.");
}


// "//" [ authority / path_abempty / path_absolute / path_rootless ]
void Request::hier_part()
{
    _and("RS", "//", OR, STAR_NO_MIN, STAR_NO_MAX , "nnnn", &authority, &path_abempty, &path_absolute, &path_rootless);
}

// authority = [ userinfo "@" ] host [ ":" port ]
void Request::authority()
{
    _and("SnS", AND, STAR_NO_MIN, STAR_NO_MAX, "nc", &userinfo, '@', &host, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", ':', &port);
}


// userinfo = *( unreserved / pct_encoded / sub_delims / ":" )
void Request::userinfo()
{
    n_star_m_or(STAR_NO_MAX, STAR_NO_MIN, "nnnc", &unreserved, &pct_encoded, &sub_delims, ':');
}

void Request::host()
{
    _or("nnn", &IP_literal, &IPv4address, &reg_name);
}

void Request::IP_literal()
{
    _and("cSc", '[', &IPv6address, &IPvFuture, ']');
}

/*
IPv6address =                            
                                   6( h16 ":" ) ls32

			/                       "::" 5( h16 ":" ) ls32

			/ [               h16 ] "::" 4( h16 ":" ) ls32

			/ [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32

			/ [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32

			/ [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32

			/ [ *4( h16 ":" ) h16 ] "::"              ls32
			/ [ *5( h16 ":" ) h16 ] "::"              h16
			/ [ *6( h16 ":" ) h16 ] "::"
*/
void Request::IPv6address()
{
    _or("aaaaaaa",
        "Sn",       AND, 6, 6, "nc", &h16, ':', &ls32,
        "RSn",      "::", AND, 5, 5, "nc", &h16, ':', &ls32,
        "sRSn",     STAR_NO_MIN, 1, &h16, "::", AND, 4, 4, "nc", &h16, ':', &ls32,
        "SRSn",     AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 1, "nc", &h16, ':', &h16, "::", AND, 3, 3, "nc", &h16, ':', &ls32,
        "SRSn",     AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 2, "nc", &h16, ':', &h16, "::", AND, 2, 2, "nc", &h16, ':', &ls32,
        "SRncn",    AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 3, "nc", &h16, ':', &h16, "::", &h16, ':', &ls32,
        "SRn",      AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 4, "nc", &h16, ':', &h16, "::", &ls32,
        "SRn",      AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 5, "nc", &h16, ':', &h16, "::", &h16,
        "SR",       AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 5, "nc", &h16, ':', &h16, "::");
}

void Request::h16()
{
    n_star_m(1, 4, &HEXDIG);
}

void Request::ls32()
{
    _or("an", "ncn", &h16, ':', &h16, &IPv4address);
}