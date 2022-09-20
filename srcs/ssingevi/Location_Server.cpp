#include "Header.hpp"

Location::Location()
: get_method(false), post_method(false), delete_method(false), redirect(),root(), autoindex(false), default_file(), CGI(), upload(false, "")
{}

Location::Location(const Location &cpy)
{
	*this = cpy;
}

Location::~Location(){}

Location &Location::operator=(const Location &cpy)
{
	get_method = cpy.get_method;
	post_method = cpy.post_method;
	delete_method = cpy.delete_method;
	redirect = cpy.redirect;
	root = cpy.root;
	autoindex = cpy.autoindex;
	default_file = cpy.default_file;
	CGI = cpy.CGI;
	upload = cpy.upload;
	return (*this);
}

Server_block::Server_block()
: port(), address(), server_names(), main(false), error_pages(), root(), body_size(), locations()
{}

Server_block::Server_block(const Server_block &cpy)
{
	*this = cpy;
}

Server_block::~Server_block()
{}

Server_block &Server_block::operator=(const Server_block &cpy)
{
	port = cpy.port;
	address = cpy.address;
	server_names = cpy.server_names;
	main = cpy.main;
	error_pages = cpy.error_pages;
	body_size = cpy.body_size;
	locations = cpy.locations;
	return (*this);
}