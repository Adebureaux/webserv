# include "Request.hpp"

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

    void Request::_and(const std::string &fct_ptr_tag, va_list arg)
    {
        std::string::const_iterator start = fct_ptr_tag.begin();
        std::string::const_iterator end = fct_ptr_tag.end();
        
        while (start != end)
        {
            switch (*start)
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
                _or(std::string(va_arg(arg, char *)), arg);
                break;
            case 'a':
                _or(std::string(va_arg(arg, char *)), arg);
                break;
            case 'n':
                (this->*va_arg(arg,pf))();
                break;
            case 'r' :
                _range(va_arg(arg, char),va_arg(arg, char));
                break;
            case 'R' :
                    is_str(va_arg(arg, char *);
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
            start++;
        }
        
    }

    
    

    void Request::_or(const std::string &fct_ptr_tag, va_list arg)
    {
        std::string::const_iterator start = fct_ptr_tag.begin();
        std::string::const_iterator end = fct_ptr_tag.end();
        pf fct;

        while (start != end)
        {
            try
            {
                switch (*start)
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
                    _or(std::string(va_arg(arg, char *)), arg);
                    break;
                case 'a':
                    _or(std::string(va_arg(arg, char *)), arg);
                    break;
                case 'n':
                     (this->*va_arg(arg,pf))();
                    break;
                case 'r' :
                    _range(va_arg(arg, char),va_arg(arg, char));
                    break;
                case 'R' :
                    is_str(va_arg(arg, char *);
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
                return;
            }
            catch (const std::invalid_argument& e)
            {
                (void)e;
            }
            start++;    
        }
        throw std::invalid_argument("error _or wrong arg");
    }

    void Request::http_message()
    {
        _and("nSns", &start_line, AND, STAR_NO_MIN, STAR_NO_MAX,  "nn", &header_field, &CRLF, &CRLF, STAR_NO_MIN, STAR_NO_MAX, &message_body);
    }

    void Request::start_line(void)
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
        is_str("GET");
    }
    void Request::_post()
    {
        is_str("POST");
    }
    void Request::_delete()
    {
        is_str("DELETE");
    }

    void Request::request_target()
    {
        _or("nnn", &origin_form, &absolute_form, &authority_form, &asterisk_form);
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
        n_star_m_or(STAR_NO_MAX, STAR_NO_MIN, "ncc", &PCHAR , '/', '?');
    }