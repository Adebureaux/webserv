#pragma once
#include <string>
#include <cstdarg>
#include <exception>
#include <stdexcept>
#include <iostream>

#define GET 0
#define POST 1
#define DELETE 2
#define NO_METHOD -1
#define OR  0
#define AND 1

#define STAR_NO_MIN     0
#define STAR_NO_MAX     -1


class Request
{
    public:
    Request(const std::string& raw_request);
    ~Request();
    std::string get_path();
    std::string get_host();
    int         get_method();
    
    private:
    
    typedef void (Request::*pf)(void);


    int         _head;
    std::string _raw_request;
    int         _method;
    std::string _host;

    /*
    fct_pointer_tag
    s/S rule * 
    o   rule |
    a   rule ' '
    n   normal fct_pointer
    r   _range
    R   is_str
    c   _is_char
    C   _is_charset
    */

    va_list *ret_copy(va_list src);
    // void expand_va_arg(va_list arg);
    void n_star_m(int n, int m,void (Request::*fct)(void));  //s                                         // ✓
    void n_star_m_or(int n, int m, ...);
    void n_star_m_or(int n, int m, va_list arg);    //S                                         // ✓
    void n_star_m_and(int n, int m, ...);
    void n_star_m_and(int n, int m, va_list arg);   //S                                         // ✓
    void _or(const std::string &param, ... );       //o                                         // ✓
    void _or(const std::string &param, va_list arg);                                            // ✓
    void _and(const std::string &param, ... );      //a                                         // ✓
    void _and(const std::string &param, va_list arg);                                           // ✓                          

    void DIGIT();
    void UPALPHA();
    void LOALPHA();
    void ALPHA();
    void CHAR();
    void PCHAR(void);
    void OCTET();    
    void CR();
    void LF();
    void CRLF();
    void SP();
    void HTAB();
    void WSP();
    void CTL();
    void OWS();
    void DQUOTE();
    void token();
    
    void _range(char start, char end);
    void _is_char(char c);
    void _is_charset(std::string charset);
    void _is_str(std::string str);

    void http_message();
        void start_line();
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
                    void authority_form();
                        void authority();
                            void userinfo();
                            void host();
                            void port();
                    void asterisk_form();
                void http_version();
                    void http_name();
        void header_field();
            void field_name();
            void field_value();
                void field_content();
                    void field_vchar();
                        void obs_text();
                void obs_fold();
        void message_body();
};

// HTTP_message = start_line *( header_field CRLF ) CRLF [ message_body ]
// void http_message(void)
// {
//      _and("nSns",start_line(), n_star_m(0,-1, _and("nn",header_field(), CRLF()), CRLF, n_star_m(0, 1, message_body())));
// }