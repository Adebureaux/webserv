# include "Request.hpp"

Request::Request(const std::string& raw_request) : 
_raw_request(raw_request), _head(0), _method(-1), _connection("keep alive"), _authority(""), _host(""), _request_target("")
{
    try
    {
        
        http_message();
    }
    catch(const std::exception& e)
    {
        std::cerr << "error req/" << e.what() << '\n';
    }
    
}

Request::~Request()
{
}

va_list *Request::ret_copy(va_list src)
{
    va_list *dst;
    // std::allocator<va_list>
    dst = (va_list*)malloc(1 * sizeof(va_list));//va_list;
    va_copy(*dst,src);
    return dst;
}

void Request::n_star_m(int n, int m, void(Request::*fct)(void))
{
    n_star_m_or(n, m, "n", fct);
}

void Request::n_star_m_or(int n, int m, ...)
{
    va_list arg;

    va_start(arg, m);
    n_star_m_or(n, m, &arg);
}

void Request::n_star_m_or(int n, int m, va_list *arg)
{
    std::string fct_ptr_tag = va_arg(*arg, char *);

    va_list *ap = ret_copy(*arg);
    int i = 0;
    try
    {
        std::cout << "----n_star_m_or" << std::endl;
        _or(fct_ptr_tag, arg);
        for (i = ((n > 0) ? 1 : 0); i != m; i++)
        {
            // std::cout << "n_star_m_or" << std::endl;
            _or(fct_ptr_tag, *ret_copy(*ap));
        }
    }
    catch(const std::exception& e)
    {
        if(i < n || (m != -1 && i > m))
            throw std::invalid_argument(e.what());
        else
            return;
    }
    
}

void Request::n_star_m_and(int n, int m, ...)
{
    va_list arg;

    va_start(arg, m);
    n_star_m_and(n, m, &arg);
}

void Request::n_star_m_and(int n, int m, va_list *arg)
{
    std::string fct_ptr_tag = va_arg(*arg, char *);

    va_list *ap = ret_copy(*arg);
    int i = 0;


    try
    {
        /* code */
        _and(fct_ptr_tag, arg);
        for (i = ((n > 0) ? 1 : 0); i != m; i++)
        {
            _and(fct_ptr_tag, *ret_copy(*ap));
        }
    }
    catch(const std::exception& e)
    {
        if(i < n || (m != -1 && i > m))
            throw std::invalid_argument(e.what());
        return;
    }
    
}

void Request::_or(const std::string &fct_ptr_tag, ...)
{
    va_list arg;

    va_start(arg, fct_ptr_tag);
    _or(fct_ptr_tag, &arg);        
}

void Request::_and(const std::string &fct_ptr_tag, ...)
{
    va_list arg;
    va_start(arg, fct_ptr_tag);
    _and(fct_ptr_tag, &arg);        
}

void Request::expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg)
{
    std::cout << "expand_va_arg it = " << *fct_it_tag << std::endl;
    switch (*fct_it_tag)
    {
        case 's':
            n_star_m(va_arg(*arg, int), va_arg(*arg, int), va_arg(*arg,void(Request::*)(void)));
            break;
        case 'S':
            switch (va_arg(*arg, int))
            {
            case AND:
                n_star_m_or(va_arg(*arg, int),va_arg(*arg, int), arg);
                break;
            case OR:
                n_star_m_or(va_arg(*arg, int),va_arg(*arg, int), arg);
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
            _is_str(va_arg(*arg, char *));
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

void Request::_and(const std::string &fct_ptr_tag, va_list *arg)
{
    std::string::const_iterator start = fct_ptr_tag.begin();
    std::string::const_iterator end = fct_ptr_tag.end();
    static int i = -1;
    
    i++;
    std::cout << "_and" << std::endl;
    while (start != end)
    {
        std::cout << i << " _and" <<std::endl;
        expand_va_arg(start, arg);
        start++;
    }
    std::cout << "_and done" << std::endl;
}

void Request::_or(const std::string &fct_ptr_tag, va_list *arg)
{
    std::string::const_iterator start = fct_ptr_tag.begin();
    std::string::const_iterator end = fct_ptr_tag.end();
    static int i = -1;
    i++;
    while (start != end)
    {
        try
        {
            std::cout << i << " _or" <<std::endl;
            expand_va_arg(start, arg);
            std::cout << "_or done" << std::endl;
            return;
        }
        catch(const std::exception& e)
        {
            start++;
            if(start >= end)
            {
                //    std::cout << i++ << " _or" <<std::endl;
                throw std::invalid_argument("error _or" + std::string(e.what()));
            }
        }
        // start++;    
    }
    // throw std::invalid_argument("error _or wrong arg");
}

void Request::CR()
{
    _is_char('\r');
}

void Request::CRLF()
{
    _and("nn", &Request::CR, &Request::LF);
}

void Request::http_message()
{
    try
    {
        std::cout << "http_message" << std::endl;
        // _head = 14444444;
        _and("nSns", &Request::start_line, AND, STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::header_field, &Request::CRLF, &Request::CRLF, STAR_NO_MIN, STAR_NO_MAX, &Request::message_body);
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << "http_message/" << e.what() << '\n';
    }
    
}

void Request::start_line()
{
    try
    {
        std::cout << "start_line" << std::endl;
        // std::cerr << e.what() << '\n';
        request_line();
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("start_line/" + std::string(e.what()));
    }
    
}

void Request::request_line()
{
    try
    {
        std::cout << "request_line" << std::endl;
        _and("nnnnnn", &Request::method, &Request::SP, &Request::request_target, &Request::SP, &Request::http_version, &Request::CRLF);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("request_line/" + std::string(e.what()));
    }
    
}

void Request::method()
{
    try
    {
        std::cout << "method" << std::endl;
        _or("nnn", &Request::_get, &Request::_post, &Request::_delete);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("method/" + std::string(e.what()));
    }
    
}

void Request::_get()
{
    std::cout << "get" << std::endl;
    _is_str("GET");
    _method = GET;
}
void Request::_post()
{
    std::cout << "post" << std::endl;
    _is_str("POST");
    _method = POST;
}
void Request::_delete()
{
    std::cout << "delete" << std::endl;
    _is_str("DELETE");
    _method = DELETE;
}

void Request::request_target()
{
    size_t old_head = _head;

    try
    {
        std::cout << "request_target" << std::endl;
        _or("nnnn", &Request::origin_form, &Request::absolute_form, &Request::authority_form, &Request::asterisk_form);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("request_target/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
    _request_target = std::string(_raw_request, old_head, _head - old_head);
}

void Request::origin_form()
{
    try
    {
        std::cout << "origin_form" << std::endl;
        _and("nS", &Request::absolute_path, AND, STAR_NO_MIN, 1, "cn", '?', &Request::query);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("origin_form/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
}

void Request::absolute_path()
{
    try
    {
        std::cout << "absolute_path" << std::endl;
        n_star_m_and(1, STAR_NO_MAX, "cn", '/', &Request::segment);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("absolute_path/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
}

void Request::segment()
{
    try
    {
        std::cout << "segment" << std::endl;
        n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::PCHAR);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("segment/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
}
void Request::query()
{
    n_star_m_or(STAR_NO_MAX, STAR_NO_MIN, "ncc", &Request::PCHAR, '/', '?');
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
    std::cout << "DIGIT" << std::endl;
    _range('0', '9');
}

void Request::HEXDIG()
{
    _or("nr", &Request::DIGIT, 'A', 'F');
}

void Request::BIT()
{
    _range('0', '1');
}

void Request::UPALPHA()
{
    std::cout << "UPALPHA" << std::endl;
    _range('A', 'Z');
}

void Request::LOALPHA()
{
    std::cout << "LOALPHA" << std::endl;
    _range('a', 'z');
}

void Request::ALPHA()
{
    std::cout << "ALPHA" << std::endl;
    _or("nn", &Request::UPALPHA, &Request::LOALPHA);
}

void Request::OCTET()
{
    _range(0x00, 0xFF);
}


void Request::_is_str(std::string const &str)
{
    if(str.compare(0, str.length(), _raw_request, _head, _head + str.length()))
        throw std::invalid_argument(str + " was expected");
    _head += str.length();
}

void Request::_range(char start, char end)
{
    std::cout << "_range between : " << start << " and " << end << std::endl;
    if (_raw_request[_head] < start || _raw_request[_head] > end)
        throw std::invalid_argument("char between '" + std::string(&start, 1) + "' and '" + std::string(&end, 1) + "' was expected");
    _head++;
}

void Request::_is_charset(std::string const &charset)
{
    std::string::const_iterator it = charset.begin();

    std::cout << "_is_charset " << charset << std::endl;
    for (; it != charset.end(); ++it)
    {
        if (*it == _raw_request[_head])
        {
            _head++;
            return;
        }
    }
    throw std::invalid_argument(charset + " was expected");
    std::cout << "charset  ok" << std::endl;
}

void Request::_is_char(char c)
{
    std::cout << "_is_char " << '\''<< c << '\''<< std::endl;
    if (_raw_request[_head] != c)
        throw std::invalid_argument("char '" + std::string(&c, 1) + "' was expected");
    _head++;
    std::cout << "is char ok" << std::endl;
}

void Request::tchar()
{
    _or("nnC", &Request::DIGIT, &Request::ALPHA, "!#$%&\'*+-.^_\\`|~");
}

void Request::PCHAR()
{
    std::cout << "pchar" << std::endl;
    try
    {
        _or("nnncc", &Request::unreserved, &Request::pct_encoded, &Request::sub_delims, ':', '@');
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("PCHAR/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
}

void Request::unreserved()
{
    std::cout << "unreserved" << std::endl;
    _or("Cnn", "-._~", &Request::DIGIT, &Request::ALPHA);
}

void Request::pct_encoded()
{
    std::cout << "pct_encoded" << std::endl;
    _and("cnn", '%', &Request::HEXDIG, &Request::HEXDIG);
}

void Request::sub_delims()
{
    std::cout << "sub_delims" << std::endl;
    _is_charset("!$&'()*+,;=");
}

void Request::absolute_form()
{
    try
    {
        std::cout << "absolute_form" << std::endl;
        absolute_URI();
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("absolute_form/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
}

void Request::asterisk_form()
{
    _is_char('*');
}

void Request::absolute_URI()
{
    try
    {
        _and("ncnS", &Request::scheme, ':', &Request::hier_part,  AND, STAR_NO_MIN, 1, "cn", '?', &Request::query);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("absolute_uri/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
}

void Request::scheme()
{
    _and("nS", &Request::ALPHA, OR, STAR_NO_MIN, STAR_NO_MAX, "nnC", &Request::ALPHA, &Request::DIGIT, "+-.");
}


// "//" [ authority / path_abempty / path_absolute / path_rootless ]
void Request::hier_part()
{
    _and("RS", "//", OR, STAR_NO_MIN, 1 , "nnnn", &Request::authority, &Request::path_abempty, &Request::path_absolute, &Request::path_rootless);
}

// authority = [ userinfo "@" ] host [ ":" port ]
void Request::authority()
{
    _and("SnS", AND, STAR_NO_MIN, 1, "nc", &Request::userinfo, '@', &Request::host, AND, STAR_NO_MIN, 1, "cn", ':', &Request::port);
}


// userinfo = *( unreserved / pct_encoded / sub_delims / ":" )
void Request::userinfo()
{
    n_star_m_or(STAR_NO_MAX, STAR_NO_MIN, "nnnc", &Request::unreserved, &Request::pct_encoded, &Request::sub_delims, ':');
}

void Request::host()
{
    size_t old_head = _head;
    _or("nnn", &Request::IP_literal, &Request::IPv4address, &Request::reg_name);
    _host = std::string(_raw_request.begin() + old_head, _raw_request.begin() + _head);

}

void Request::IP_literal()
{
    _and("cSc", '[', &Request::IPv6address, &Request::IPvFuture, ']');
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
        "Sn",       AND, 6, 6, "nc", &Request::h16, ':', &Request::ls32,
        "RSn",      "::", AND, 5, 5, "nc", &Request::h16, ':', &Request::ls32,
        "sRSn",     STAR_NO_MIN, 1, &Request::h16, "::", AND, 4, 4, "nc", &Request::h16, ':', &Request::ls32,
        "SRSn",     AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 1, "nc", &Request::h16, ':', &Request::h16, "::", AND, 3, 3, "nc", &Request::h16, ':', &Request::ls32,
        "SRSn",     AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 2, "nc", &Request::h16, ':', &Request::h16, "::", AND, 2, 2, "nc", &Request::h16, ':', &Request::ls32,
        "SRncn",    AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 3, "nc", &Request::h16, ':', &Request::h16, "::", &Request::h16, ':', &Request::ls32,
        "SRn",      AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 4, "nc", &Request::h16, ':', &Request::h16, "::", &Request::ls32,
        "SRn",      AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 5, "nc", &Request::h16, ':', &Request::h16, "::", &Request::h16,
        "SR",       AND , STAR_NO_MIN, 1, "Sn", AND , STAR_NO_MIN, 5, "nc", &Request::h16, ':', &Request::h16, "::");
}

void Request::h16()
{
    n_star_m(1, 4, &Request::HEXDIG);
}

void Request::ls32()
{
    _or("an", "ncn", &Request::h16, ':', &Request::h16, &Request::IPv4address);
}


// IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub_delims / ":" )
void Request::IPvFuture()
{
    _and("cscS", 'v', 1, STAR_NO_MAX, &Request::HEXDIG, '.', OR, 1, STAR_NO_MAX, "nnc", &Request::unreserved, &Request::sub_delims, ':');
}


// IPv4address = dec_octet "." dec_octet "." dec_octet "." dec_octet


void Request::IPv4address()
{
    _and("ncncncn", &Request::dec_octet, '.', &Request::dec_octet, '.', &Request::dec_octet, '.', &Request::dec_octet);
}

// dec_octet   = DIGIT                 ; 0-9
// 			/ 0x31-39 DIGIT         ; 10-99   ; 1-9 0-9
// 			/ "1" 2DIGIT            ; 100-199 ; "1" 0-9 0-9
// 			/ "2" 0x30-34 DIGIT     ; 200-249 ; "2" 0-4 0-9
// 			/ "25" 0x30-35          ; 250-255 ; "25" 0-5

void Request::dec_octet()
{
    _or("naaaa", &Request::DIGIT, "rn", '1', '9', &Request::DIGIT,"cnn", '1', &Request::DIGIT, &Request::DIGIT, "crn", '2', '0', '4', &Request::DIGIT, "Rr", "25", '0', '5');
}


// reg_name = *( unreserved / pct_encoded / sub_delims )

void Request::reg_name()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nnn", &Request::unreserved, &Request::pct_encoded, &Request::sub_delims);
}

void Request::port()
{
    n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::DIGIT);
}

void Request::path_abempty()
{
    n_star_m_and(STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
}

// path_absolute = "/" [ segment_nz *( "/" segment ) ]

void Request::path_absolute()
{
    _and("cS", "/", AND, STAR_NO_MIN, 1, "nS", &Request::segment_nz, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
}

// segment_nz = 1*(pchar)

void Request::segment_nz()
{
    n_star_m(1, STAR_NO_MAX, &Request::PCHAR);
}

void Request::path_rootless()
{
    _and("nS", &Request::segment_nz, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
}

void Request::http_version()
{
    _and("ncscs", &Request::http_name, '/', 1, STAR_NO_MAX, &Request::DIGIT, '.', 1, STAR_NO_MAX, &Request::DIGIT);
}

void Request::http_name()
{
    _is_str("HTTP");
}

void Request::authority_form()
{
    authority();
}

// header_field = field_name ":" OWS field_value OWS

void Request::header_field()
{
    size_t old_head = _head;
    try
    {
        _and("ncnnn", &Request::field_name, ':', &Request::OWS, &Request::field_value, &Request::OWS);
        /* code */
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("header_field/" + std::string(e.what()));
        // std::cerr << e.what() << '\n';
    }
    
    std::string str("connection");
    if (!str.compare(0, str.length(), &Request::_raw_request[old_head], old_head, old_head + str.length()))
    {
        _head = old_head;
        field_name();
        _is_char(':');
        OWS();
        old_head = _head;
        field_value();
        _connection = std::string(_raw_request.begin() + old_head, _raw_request.begin() + _head);
        OWS();
    }


}

void Request::field_name()
{
    token();
}

// OWS = *( SP / HTAB )

void Request::OWS()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB);
}

// field_value = *( field_content / obs_fold )

void Request::field_value()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::field_content, &Request::obs_fold);
}

// field_content = field_vchar [ 1*( SP / HTAB ) field_vchar ]

void Request::field_content()
{
    _and("nS", &Request::field_vchar, AND, STAR_NO_MIN, 1, "Sn", OR, 1, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB, &Request::field_vchar);
}

// field_vchar = VCHAR / obs_text

void Request::field_vchar()
{
    _or("nn", &Request::VCHAR, &Request::obs_text);
}

void Request::VCHAR()
{
    _range(0x21, 0x7e);
}

void Request::obs_text()
{
    _range(0x80, 0xFF);
}

// obs_fold = CRLF 1*( SP / HTAB )

void Request::obs_fold()
{
    _and("nS", &Request::CRLF, AND, 1, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB);
}

void Request::token()
{
    n_star_m(1, STAR_NO_MAX, &Request::tchar);
}

void Request::message_body()
{
    n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::OCTET);
}