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
		Request( const Request &thread )
		{
			*this = thread;
		}

		//operator =
		Request& operator=( const Request & thread)
		{
			this->type = thread.type;
			this->host_ip = thread.host_ip;
			this->host_address = thread.host_address;
			this->page = thread.page;
			this->error_type = thread.error_type;
			this->status_is_finished = thread.status_is_finished;
			this->status_is_handled = thread.status_is_handled;
			this->content_type = thread.content_type;
			this->encoding = thread.encoding;
			this->content_size = thread.content_size;
			this->body = thread.body;
			this->is_cgi = thread.is_cgi;
			this->query = thread.query;
			this->pure_content = thread.pure_content;
			this->filename = thread.filename;
			return (*this);
		}
	
}; // request

////////////////////////////////////////////////////////////////
//                      Response                   //
////////////////////////////////////////////////////////////////

class Response
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
		Response() {}

		//operator =
		Response& operator=( const Response & thread)
		{
			if (this == &thread)
				return (*this);
			this->fd_accept = thread.fd_accept;
			this->fd_read = thread.fd_read;
			this->fd_write = thread.fd_write;
			this->fd_listen = thread.fd_listen;
			this->specs = thread.specs;
			this->re = thread.re;
			this->loc = thread.loc;
			this->absolut_path = thread.absolut_path;
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

}; // Response


void		go_request(char *msg, Request *r);
void		chunked(std::string mess, Request *r);
std::string	get_filename(std::string mess);

#endif