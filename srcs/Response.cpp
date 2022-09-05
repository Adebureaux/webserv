#include "Response.hpp"

void set_cgi_env(
	const char *script_path,
	const char *query,
	const char *path_translated,
	const char *server_port,
	const char *request_method,
	const char *remote_addr
)
{
	setenv("SERVER_SOFTWARE", "webserv/1.0", 1); // Le nom et la version du serveur HTTP répondant à la requête. (Format : nom/version)
	setenv("SERVER_NAME", "webserv", 1); // Le nom d'hôte, alias DNS ou adresse IP du serveur.
	setenv("GATEWAY_INTERFACE", "PHP/7.4.3", 1); // La révision de la spécification CGI que le serveur utilise. (Format : CGI/révision)
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1); // Le nom et la révision du protocole dans lequel la requête a été faite (Format : protocole/révision)
	setenv("SERVER_PORT", server_port, 1); // Le numéro de port sur lequel la requête a été envoyée.
	setenv("REQUEST_METHOD", request_method, 1); // La méthode utilisée pour faire la requête. Pour HTTP, elle contient généralement « GET » ou « POST ».
	setenv("QUERY_STRING", query, 1); // Contient tout ce qui suit le « ? » (non inclus) dans l'URL envoyée par le client. Toutes les variables provenant d'un formulaire envoyé avec la méthode « GET » seront contenues dans le QUERY_STRING sous la forme « var1=val1&var2=val2&... ».
	setenv("PATH_TRANSLATED", path_translated, 1); // Query string stripped, Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.
	setenv("REMOTE_ADDR", remote_addr, 1); // L'adresse IP du client.
	setenv("HTTP_ACCEPT", "text/html", 1); // Les types de données MIME que le client accepte de recevoir. Exemple : text/*, image/jpeg, image/png, image/*, */*
	setenv("HTTP_ACCEPT_LANGUAGE", "en-US,en;q=0.9", 1); // Les langues dans lequel le client accepte de recevoir la réponse. Exemple : fr_CA, fr
	setenv("HTTP_USER_AGENT", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0 Safari/537.36", 1); // Le navigateur utilisé par le client. Exemple : Mozilla/5.0 (compatible; Konqueror/3; Linux)
	setenv("REDIRECT_STATUS", "200", 1); // truc de securite pour php
}
const char* cgi(const char *script_path) {
	int out[2], error[2], pid, status;
	static char buffer_out[2048];
	static char buffer_error[2048];

	pipe(out);
	pipe(error);
	if ((pid = fork()) == -1)
		std::cout << "error: fork()\n";
	if (pid == 0)
	{

		close(out[0]);
		close(error[0]);
		dup2(out[1], 1);
		dup2(error[1], 2);
		close(out[1]);
		close(error[1]);
		set_cgi_env();
		execl("/usr/bin/php-cgi", "/usr/bin/php-cgi", script_path, 0);
		exit(1);
	}
	waitpid(pid, &status, 0);
	close(out[1]);
	close(error[1]);
	while (read(out[0], buffer_out, sizeof(buffer_out)) != 0) {}
	close(out[0]);
	while (read(error[0], buffer_error, sizeof(buffer_error)) != 0) {}
	close(error[0]);
	std::cout<< GREEN << "CGI OUT:\n" << buffer_out << CLEAR;
	std::cout<<std::endl << RED << "CGI ERROR:" << status << "\n" << buffer_error << CLEAR;
	return 0;
}


Response::Response() {}

Response::~Response() {}

void Response::respond(const Request& request) {
	if (request.getMethod() == "GET")
		get(request);
	debug();
}

void Response::get(const Request &request) {
	std::ifstream file(request.getPath().c_str());
	std::stringstream ssbuffer;
	std::string buffer;

	ssbuffer << file.rdbuf();
	file.close();
	_response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
	body = ssbuffer.str();
	_response.append(SSTR("" << body.size()));
	_response.append("\n\n");
	_response.append(body);
}

void Response::clear(void) {
	_response.clear();
}

const std::string& Response::send(void) {
	return (_response);
}

void Response::debug(void) {
	std::cerr << "\033[1;32mResponse : \n" << _response << "\033[0m" << std::endl;
}
