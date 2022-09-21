#pragma once
#include "Shared.hpp"

#define EXECP					std::invalid_argument(std::string(__FUNCTION__) + "/"+ std::string(e.what()))
#define STAR_NO_MIN				 0
#define STAR_NO_MAX				-1
#define OR						 1
#define AND						 0

class Parser
{
	public :
	Parser();
	Parser(const std::string& str);
	Parser(const Parser &cpy);
	Parser &operator=(const Parser &src);
	virtual ~Parser();

	protected :
	size_t								_head;
	std::string							_raw_str;

	void ft_va_copy(va_list *dest, va_list *src);																		// ✓
	virtual void expand_va_arg(std::string::const_iterator &fct_it_tag, va_list *arg) = 0;								// ✓
	virtual void finish_expand(std::string::const_iterator start, std::string::const_iterator end, va_list *arg) = 0;	// ✓
	void _range(char start, char end);																					// ✓
	void _is_char(char c);																								// ✓
	void _is_charset(std::string const &charset);																		// ✓
	void _is_str(std::string const &str);																				// ✓
	void n_star_m(int n, int m,void (Parser::*fct)(void));	// s												// ✓
	void n_star_m_or(int n, int m, ...);																				// ✓
	void n_star_m_or(int n, int m, va_list *arg);				// S													// ✓
	void n_star_m_and(int n, int m, ...);																				// ✓
	void n_star_m_and(int n, int m, va_list *arg);				// S													// ✓
	void _or(const std::string param, ... );					// o													// ✓
	void _or(const std::string &param, va_list *arg);																	// ✓
	void _and(const std::string param, ... );					// a													// ✓
	void _and(const std::string &param, va_list *arg);

	void DIGIT();				// ✓
	void BIT();					// ✓
	void UPALPHA();				// ✓
	void LOALPHA();				// ✓
	void ALPHA();				// ✓
	void CHAR();				// ✓
	void OCTET();				// ✓
	void CR();					// ✓
	void LF();					// ✓
	void CRLF();				// ✓
	void SP();					// ✓
	void HTAB();				// ✓
	void WSP();					// ✓
	void CTL();					// ✓
	void OWS();					// ✓
	void DQUOTE();				// ✓
	void token();				// ✓
	void HEXDIG();				// ✓
	void tchar();				// ✓
	void scheme();				// ✓
	void PCHAR();				// ✓
		void unreserved();		// ✓
		void pct_encoded();		// ✓
		void sub_delims();		// ✓
};
