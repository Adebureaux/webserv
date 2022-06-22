#ifndef WEBSERV_REQUEST_HPP
#define WEBSERV_REQUEST_HPP

#include <iostream>
#include <cstdlib>

////////////////////////////////////////////////////////////////
//                          REQUEST                           //
////////////////////////////////////////////////////////////////

class Request
{
	public:

		std::string type;			// GET, POST, DELETE...
		std::string host_ip;		// port (8082, 8080...)
		std::string host_address;	// 17.0.0.1 exaple.com
		std::string page;			// /webpages/index.html
		int error_type;				// error num
		bool status_is_handled;		// indique si requete en cours de reponse
		bool status_is_finished;	// indique si requete finie
		std::string content_type;	// indique type de contenu reçu (GET, POST, DELETE)
		std::string encoding;		// indique type de encodage (GET, POST, DELETE)
		long content_size;			// taille du message
		std::string body;			// message
		bool is_cgi;
		std::string query;
		std::string pure_content;
		std::string filename;

		//default constructor
		Request()
		{
			this->type = "";
			this->host_ip = "";
			this->host_address = "";
			this->page = "";
			this->error_type = 200;
			this->status_is_finished = false;
			this->status_is_handled = false;
			this->content_type = "";
			this->encoding = "";
			this->content_size = 0;
			this->body = "";
			this->is_cgi = false;
			std::string query = "";
			std::string pure_content = "";
			std::string filename = "";
		}

		//copy constructor
		Request( const Request &one )
		{
			*this = one;
		}

		//operator =
		Request& operator=( const Request & one)
		{
			this->type = one.type;
			this->host_ip = one.host_ip;
			this->host_address = one.host_address;
			this->page = one.page;
			this->error_type = one.error_type;
			this->status_is_finished = one.status_is_finished;
			this->status_is_handled = one.status_is_handled;
			this->content_type = one.content_type;
			this->encoding = one.encoding;
			this->content_size = one.content_size;
			this->body = one.body;
			this->is_cgi = one.is_cgi;
			this->query = one.query;
			this->pure_content = one.pure_content;
			this->filename = one.filename;
			return (*this);
		}
	
}; // request

////////////////////////////////////////////////////////////////
//                      BUNDLE_FOR_RESPONSE                   //
////////////////////////////////////////////////////////////////

class Bundle_for_response
{

	public:

		Request re;

		int fd_listen;
		int fd_accept;
		int fd_read;
		int fd_write;
		int specs;

		std::string loc;
		std::string absolut_path;

		//default constructor
		Bundle_for_response() {}

		//operator =
		Bundle_for_response& operator=( const Bundle_for_response & one)
		{
			if (this == &one)
				return (*this);
			this->fd_accept = one.fd_accept;
			this->fd_read = one.fd_read;
			this->fd_write = one.fd_write;
			this->fd_listen = one.fd_listen;
			this->specs = one.specs;
			this->re = one.re;
			this->loc = one.loc;
			this->absolut_path = one.absolut_path;
			return (*this);
		}

		//initialization
		void init_re()
		{
			this->re.type = "";
			this->re.host_ip = "";
			this->re.host_address = "";
			this->re.page = "";
			this->re.error_type = 200;
			this->re.status_is_finished = false;
			this->re.status_is_handled = false;
			this->re.content_type = "";
			this->re.encoding = "";
			this->re.content_size = 0;
			this->re.body = "";
			this->re.is_cgi = false;
			this->re.query = "";
			this->re.pure_content = "";
			this->re.filename = "";
		}

}; // Bundle_for_response


void		first_dispatch(char *msg, Request *r);
void		chunked_post(std::string mess, Request *r);
std::string	get_filename(std::string mess);

#endif