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

#define OR  1
#define AND 0

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
    void        set_raw_request(const std::string& raw_request);
    
    private:
    
    typedef void (Request::*pf)(void);

    // _raw_request(raw_request), _head(0), _method(-1), _connection("keep alive"), _authority(""), _host(""), _request_target("")
    std::string _raw_request;
    size_t      _head;
    int         _method;
    std::string _connection;
    std::string _authority;
    std::string _host;
    std::string _request_target;

    /*
    fct_pointer_tag
    s/S rule * 
    o   rule |
    a   rule ' '
    n   normal fct_pointer
    r   _range
    R   _is_str
    c   _is_char
    C   _is_charset
    */

    va_list *ret_copy(va_list src);                                                             // ✓
    void expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg);                   // ✓
    void finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg);
    void _range(char start, char end);                                                          // ✓
    void _is_char(char c);                                                                      // ✓
    void _is_charset(std::string const &charset);                                               // ✓
    void _is_str(std::string const &str);                                                       // ✓
    void n_star_m(int n, int m,void (Request::*fct)(void));  //s                                // ✓
    void n_star_m_or(int n, int m, ...);                                                        // ✓
    void n_star_m_or(int n, int m, va_list *arg);    //S                                         // ✓
    void n_star_m_and(int n, int m, ...);                                                       // ✓
    void n_star_m_and(int n, int m, va_list *arg);   //S                                         // ✓
    void _or(const std::string &param, ... );       //o                                         // ✓
    void _or(const std::string &param, va_list *arg);                                            // ✓
    void _and(const std::string &param, ... );      //a                                         // ✓
    void _and(const std::string &param, va_list *arg);                                           // ✓                          

    void DIGIT();               // ✓
    void BIT();                 // ✓
    void UPALPHA();             // ✓
    void LOALPHA();             // ✓
    void ALPHA();               // ✓  
    void CHAR();
    void PCHAR();               // ✓
        void unreserved();
        void pct_encoded();
        void sub_delims();
    void OCTET();    
    void CR();                  // ✓
    void LF();                  // ✓
    void CRLF();                // ✓
    void SP();                  // ✓
    void HTAB();                // ✓
    void WSP();
    void CTL();
    void OWS();
    void DQUOTE();
    void token();
    void HEXDIG();  // ✓
    void tchar();
    void scheme();
    

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
                        void hier_part();
                            void path_abempty();
                            void path_absolute();
                                void segment_nz();
                            void path_rootless();
                    void authority_form();
                        void authority();
                            void userinfo();
                            void host();
                                void IP_literal();
                                    void IPv6address();
                                        void h16();
                                        void ls32();
                                    void IPvFuture();
                                void IPv4address();
                                    void dec_octet();
                                void reg_name();
                            void port();
                    void asterisk_form();
                void http_version();
                    void http_name();
        void header_field();
            void field_name();
            void field_value();
                void field_content();
                    void field_vchar();
                        void VCHAR();
                        void obs_text();
                    void obs_fold();
                // void obs_fold();
        void message_body();
};

// HTTP_message = start_line *( header_field CRLF ) CRLF [ message_body ]
// void http_message(void)
// {
//      _and("nSns",start_line(), n_star_m(0,-1, _and("nn",header_field(), CRLF()), CRLF, n_star_m(0, 1, message_body())));
// }