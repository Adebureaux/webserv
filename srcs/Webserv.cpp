#include "Webserv.hpp"

Webserv::Webserv()
{

}

Webserv::Webserv(const Webserv &other)
{
	*this = other;
}

Webserv &Webserv::operator=(const Webserv &other)
{
	(void)other;
	return (*this);
}

Webserv::~Webserv()
{

}
