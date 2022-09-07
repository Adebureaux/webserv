# include "Request.hpp"

Request::Request(const std::string& raw_request) : 
_raw_request(raw_request), _head(0), _method(NO_METHOD), _head_msg_body(0), _var_map()
{
    try
    {
        
        http_message();
        std::cout << "http_message OK" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "error req/" << e.what() << '\n';
        std::cout << "http_message KO" << std::endl;
        std::cout << std::endl << "'"<< &_raw_request[_head] << "'"<< std::endl;
    }
}

Request::~Request()
{
}

va_list *Request::ret_copy(va_list src)
{
    va_list *dst;
    
    dst = (va_list*)malloc(1 * sizeof(va_list));
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
    va_end(arg);
}

void Request::n_star_m_or(int n, int m, va_list *arg)
{
    std::string fct_ptr_tag = va_arg(*arg, char *);
    int i;
    va_list ap;

    try
    {
        for (i = 0; i != m; i++)
        {
            va_copy(ap, *arg);
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

void Request::n_star_m_and(int n, int m, ...)
{
    va_list arg;

    va_start(arg, m);
    n_star_m_and(n, m, &arg);
    va_end(arg);
}

void Request::n_star_m_and(int n, int m, va_list *arg)
{
    std::string fct_ptr_tag = va_arg(*arg, char *);
    va_list ap;
    int i = 0;
    try
    {
        for (i = 0; i != m; i++)
        {
            va_copy(ap, *arg);
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

void Request::_or(const std::string &fct_ptr_tag, ...)
{
    va_list arg;

    va_start(arg, fct_ptr_tag);
    _or(fct_ptr_tag, &arg);
    va_end(arg);        
}

void Request::_and(const std::string &fct_ptr_tag, ...)
{
    va_list arg;
    va_start(arg, fct_ptr_tag);
    _and(fct_ptr_tag, &arg);
    va_end(arg);        
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
            n = va_arg(*arg, int);
            m = va_arg(*arg, int);
            switch (va_arg(*arg, int))
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

void Request::_and(const std::string &fct_ptr_tag, va_list *arg)
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

void Request::_or(const std::string &fct_ptr_tag, va_list *arg)
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
                throw std::invalid_argument("error _or" + std::string(e.what()));
            }
        }
        start++;  
    }
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
        _and("nSns", &Request::start_line, AND, STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::header_field, &Request::CRLF, &Request::CRLF, STAR_NO_MIN, STAR_NO_MAX, &Request::message_body);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("http_message/" + std::string(e.what()));
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
        throw std::invalid_argument("start_line/" + std::string(e.what()));
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
        throw std::invalid_argument("request_line/" + std::string(e.what()));
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
        throw std::invalid_argument("method/" + std::string(e.what()));
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
        _var_map["Request_target"] = std::string(_raw_request.begin()+ old_head,_raw_request.begin() + _head);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("request_target/" + std::string(e.what()));
    }
    
}

void Request::origin_form()
{
    try
    {
        _and("nS", &Request::absolute_path, AND, STAR_NO_MIN, 1, "cn", '?', &Request::query);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("origin_form/" + std::string(e.what()));
    }
    
}

void Request::absolute_path()
{
    try
    {
        n_star_m_and(1, STAR_NO_MAX, "cn", '/', &Request::segment);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("absolute_path/" + std::string(e.what()));
    }
    
}

void Request::segment()
{
    try
    {
        n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::PCHAR);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("segment/" + std::string(e.what()));
    }
    
}
void Request::query()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "ncc", &Request::PCHAR, '/', '?');
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
    _or("nr", &Request::DIGIT, 'A', 'F');
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
    _or("nn", &Request::UPALPHA, &Request::LOALPHA);
}

void Request::OCTET()
{
    _range(0x00, 0xFF);
}

void Request::_is_str(std::string const &str)
{
    if(str.compare(0, str.length(), _raw_request, _head, str.length()))
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
    std::cout << "_is_char " << '\''<< c << '\''<< std::endl;
    if (_raw_request[_head] != c)
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
    _head++;
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
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("PCHAR/" + std::string(e.what()));
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
        absolute_URI();
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("absolute_form/" + std::string(e.what()));
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
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("absolute_uri/" + std::string(e.what()));
    }
    
}

void Request::scheme()
{
    _and("nS", &Request::ALPHA, OR, STAR_NO_MIN, STAR_NO_MAX, "nnC", &Request::ALPHA, &Request::DIGIT, "+-.");
}

void Request::hier_part()
{
    _and("RS", "//", OR, STAR_NO_MIN, 1 , "nnnn", &Request::authority, &Request::path_abempty, &Request::path_absolute, &Request::path_rootless);
}

void Request::authority()
{
    size_t old_head = _head;

    _and("SnS", AND, STAR_NO_MIN, 1, "nc", &Request::userinfo, '@', &Request::host, AND, STAR_NO_MIN, 1, "cn", ':', &Request::port);
    _var_map["Authority"] = std::string(_raw_request.begin() + old_head, _raw_request.begin() + _head);
}

void Request::userinfo()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nnnc", &Request::unreserved, &Request::pct_encoded, &Request::sub_delims, ':');
}

void Request::host()
{
    _or("nnn", &Request::IP_literal, &Request::IPv4address, &Request::reg_name);
}

void Request::IP_literal()
{
    _and("cSc", '[', &Request::IPv6address, &Request::IPvFuture, ']');
}

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

void Request::IPvFuture()
{
    _and("cscS", 'v', 1, STAR_NO_MAX, &Request::HEXDIG, '.', OR, 1, STAR_NO_MAX, "nnc", &Request::unreserved, &Request::sub_delims, ':');
}

void Request::IPv4address()
{
    _and("ncncncn", &Request::dec_octet, '.', &Request::dec_octet, '.', &Request::dec_octet, '.', &Request::dec_octet);
}

void Request::dec_octet()
{
    _or("naaaa", &Request::DIGIT, "rn", '1', '9', &Request::DIGIT,"cnn", '1', &Request::DIGIT, &Request::DIGIT, "crn", '2', '0', '4', &Request::DIGIT, "Rr", "25", '0', '5');
}

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

void Request::path_absolute()
{
    _and("cS", "/", AND, STAR_NO_MIN, 1, "nS", &Request::segment_nz, AND, STAR_NO_MIN, STAR_NO_MAX, "cn", '/', &Request::segment);
}

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

void Request::header_field()
{
    size_t old_head = _head;
    try
    {
        _and("ncnnn", &Request::field_name, ':', &Request::OWS, &Request::field_value, &Request::OWS);
    }
    catch(const std::exception& e)
    {
        throw std::invalid_argument("header_field/" + std::string(e.what()));
    }
    catch_var_header_field(old_head);
    std::cout << "end Request::header_field head = " << _head << std::endl;
}

void Request::catch_var_header_field(size_t old_head)
{
    std::string var_name;
    _head = old_head;
    field_name();
    var_name = std::string(_raw_request.begin() + old_head, _raw_request.begin() + _head);
    OWS();
    old_head = _head;
    field_value();
    _var_map[var_name] = std::string(_raw_request.begin() + old_head, _raw_request.begin() + _head);
    OWS();
}

void Request::field_name()
{
    token();
}

void Request::OWS()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB);
}

void Request::field_value()
{
    n_star_m_or(STAR_NO_MIN, STAR_NO_MAX, "nn", &Request::field_content, &Request::obs_fold);
}

void Request::field_content()
{
    _and("nS", &Request::field_vchar, AND, STAR_NO_MIN, 1, "Sn", OR, 1, STAR_NO_MAX, "nn", &Request::SP, &Request::HTAB, &Request::field_vchar);
}

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

void Request::token()
{
    n_star_m(1, STAR_NO_MAX, &Request::tchar);
}

void Request::message_body()
{
    _head_msg_body = _head;
    throw std::invalid_argument("message_body");
    n_star_m(STAR_NO_MIN, STAR_NO_MAX, &Request::OCTET);
}

void        Request::set_raw_request(const std::string& raw_request)
{
    _raw_request = raw_request;
    _head = 0;
    _method = NO_METHOD;
    _var_map.clear();
}

int Request::get_method()
{
    return (_method);
}

std::string const Request::get_raw_request()
{
    return (_raw_request);
}

std::string const Request::get_connection()
{
    return (get_var_by_name("Connection").second);
}

std::string const Request::get_authority()
{
    return (get_var_by_name("Authority").second);
}

std::string const Request::get_host()
{
    return (get_var_by_name("Host").second);
}

std::string const Request::get_request_target()
{
    return (get_var_by_name("Request_target").second);
}

std::string const Request::get_message_body()
{
    return (std::string(_raw_request.begin() + _head_msg_body, _raw_request.end()));
}

std::pair<bool, std::string> Request::get_var_by_name(const std::string &name)
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