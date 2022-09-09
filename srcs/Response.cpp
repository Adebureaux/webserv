#include "Response.hpp"

#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
# include <string>
#include <stdio.h>
#include <unistd.h>
# include <sstream>
# include <iostream>
#include <cstring>

Response::Response() {}

Response::~Response() {}
// void set_cgi_env2(
// 	const char *server_software,
// 	const char *hostname,
// 	const char *protocol,
// 	const char *server_port,
// 	const char *request_method,
// 	const char *query,
// 	const char *script_path,
// 	const char *path_translated,
// 	const char *remote_addr,
// 	const char *http_accept,
// 	const char *cgi_revision,
// 	const char *http_accept_language,
// 	const char *http_user_agent
// )
// {
// 	setenv("SERVER_SOFTWARE",server_software, 1); // Le nom et la version du serveur HTTP répondant à la requête. (Format : nom/version)
// 	setenv("SERVER_NAME", hostname, 1); // Le nom d'hôte, alias DNS ou adresse IP du serveur.
// 	setenv("SERVER_PROTOCOL", protocol, 1); // Le nom et la révision du protocole dans lequel la requête a été faite (Format : protocole/révision)
// 	setenv("SERVER_PORT", server_port, 1); // Le numéro de port sur lequel la requête a été envoyée.
// 	setenv("REQUEST_METHOD", request_method, 1); // La méthode utilisée pour faire la requête. Pour HTTP, elle contient généralement « GET » ou « POST ».
// 	setenv("QUERY_STRING", query, 1); // Contient tout ce qui suit le « ? » (non inclus) dans l'URL envoyée par le client. Toutes les variables provenant d'un formulaire envoyé avec la méthode « GET » seront contenues dans le QUERY_STRING sous la forme « var1=val1&var2=val2&... ».
// 	setenv("PATH_TRANSLATED", path_translated, 1); // Query string stripped, Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.
// 	setenv("REMOTE_ADDR", remote_addr, 1); // L'adresse IP du client.
// 	setenv("HTTP_ACCEPT", http_accept, 1); // Les types de données MIME que le client accepte de recevoir. Exemple : text/*, image/jpeg, image/png, image/*, */*
// 	setenv("GATEWAY_INTERFACE", cgi_revision, 1); // La révision de la spécification CGI que le serveur utilise. (Format : CGI/révision)
// 	setenv("HTTP_ACCEPT_LANGUAGE", http_accept_language, 1); // Les langues dans lequel le client accepte de recevoir la réponse. Exemple : fr_CA, fr
// 	setenv("HTTP_USER_AGENT", http_user_agent, 1); // Le navigateur utilisé par le client. Exemple : Mozilla/5.0 (compatible; Konqueror/3; Linux)
// 	setenv("REDIRECT_STATUS", "200", 1); // truc de securite pour php
// }
static void set_cgi_env(const char* path)
{
	setenv("SERVER_SOFTWARE", "webserv/1.0", 1); // Le nom et la version du serveur HTTP répondant à la requête. (Format : nom/version)
	setenv("SERVER_NAME", "localhost", 1); // Le nom d'hôte, alias DNS ou adresse IP du serveur.
	setenv("GATEWAY_INTERFACE", "PHP/7.4.3", 1); // La révision de la spécification CGI que le serveur utilise. (Format : CGI/révision)
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1); // Le nom et la révision du protocole dans lequel la requête a été faite (Format : protocole/révision)
	setenv("SERVER_PORT", "8080", 1); // Le numéro de port sur lequel la requête a été envoyée.
	setenv("REQUEST_METHOD", "GET", 1); // La méthode utilisée pour faire la requête. Pour HTTP, elle contient généralement « GET » ou « POST ».
	setenv("QUERY_STRING", "name=Romain", 1); // Contient tout ce qui suit le « ? » dans l'URL envoyée par le client. Toutes les variables provenant d'un formulaire envoyé avec la méthode « GET » seront contenues dans le QUERY_STRING sous la forme « var1=val1&var2=val2&... ».
	setenv("PATH_TRANSLATED", path, 1); // Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.
	setenv("REMOTE_ADDR", "127.0.0.1", 1); // L'adresse IP du client.
	setenv("HTTP_ACCEPT", "text/html", 1); // Les types de données MIME que le client accepte de recevoir. Exemple : text/*, image/jpeg, image/png, image/*, */*
	setenv("HTTP_ACCEPT_LANGUAGE", "en-US,en;q=0.9", 1); // Les langues dans lequel le client accepte de recevoir la réponse. Exemple : fr_CA, fr
	setenv("HTTP_USER_AGENT", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0 Safari/537.36", 1); // Le navigateur utilisé par le client. Exemple : Mozilla/5.0 (compatible; Konqueror/3; Linux)
	setenv("REDIRECT_STATUS", "200", 1); // truc de securite pour php
	// setenv("PATH_INFO", "/", 1); // Le chemin supplémentaire du script tel que donné par le client. Par exemple, si le serveur héberge le script « /cgi-bin/monscript.cgi » et que le client demande l'url « http://serveur.org/cgi-bin/monscript.cgi/marecherche », alors PATH_INFO contiendra « marecherche ».
	// setenv("SCRIPT_NAME", "index.php", 1); // Le chemin virtuel vers le script étant exécuté. Exemple : « /cgi-bin/script.cgi »
	// setenv("REMOTE_HOST", "", 1);  // Le nom d'hôte du client. Si le serveur ne possède pas cette information (par exemple, lorsque la résolution DNS inverse est désactivée), REMOTE_HOST sera vide.
	// setenv("AUTH_TYPE", "", 1); // Le type d'identification utilisé pour protéger le script (s’il est protégé et si le serveur supporte l'identification).
	// setenv("REMOTE_USER", "", 1); // Le nom d'utilisateur du client, si le script est protégé et si le serveur supporte l'identification.
	// setenv("REMOTE_IDENT", "", 1); // Nom d'utilisateur (distant) du client faisant la requête. Le serveur doit supporter l'identification RFC 931. Cette variable devrait être utilisée à des fins de journaux seulement.
	// setenv("CONTENT_TYPE", "", 1); // Le type de contenu attaché à la requête, si des données sont attachées (comme lorsqu'un formulaire est envoyé avec la méthode « POST »)
	// setenv("CONTENT_LENGTH", "", 1); // La longueur du contenu envoyé par le client.
	// setenv("HTTP_COOKIE", "", 1); // Les éventuels cookies. Une liste de paires clef=valeur contenant les cookies positionnés par le site, séparés par des points-virgules.
	// setenv("HTTP_REFERER", "/", 1); // Une adresse absolue ou partielle de la page web à partir de laquelle la requête vers la page courante a été émise.
}

static std::string readToString(int fd)
{
	static char buffer[2048];
	std::stringstream res;
	int ret = 0;
	while ((ret = read(fd, buffer, sizeof(buffer))) != 0)
	{
		res << buffer;
		std::memset(buffer, 0, ret);
	}
	close(fd);
	return res.str();
}

static const std::string cgi(const std::string &script_path) {
	int out[2], error[2], pid, status;
	// static char buffer_out[2048];
	// static char buffer_error[2048];

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
		set_cgi_env(script_path.c_str());
		execl("/usr/bin/php-cgi", "/usr/bin/php-cgi", script_path.c_str(), 0);
		exit(1);
	}
	waitpid(pid, &status, 0);
	close(out[1]);
	close(error[1]);
	const std::string result = readToString(out[0]);
	// std::cout<< GREEN << "CGI OUT:\n" << readToString(out[0]) << CLEAR;
	// std::cout<<std::endl << RED << "CGI ERROR:" << status << "\n" << readToString(error[0]) << CLEAR;
	return result;
}
void Response::respond(const Request& request) {
	if (request.getMethod() == "GET")
		get(request);
	debug();
}

void Response::get(const Request &request) {
	if (request.getPath() == "index.php")
	{
		// std::cout << "requested:" << request.getPath() << std::endl;
		_response = "HTTP/1.1 200 OK\n";
		const std::string result(cgi(request.getPath()));
		_response.append(cgi(request.getPath()));
	}
	else
	{
		std::ifstream file(request.getPath().c_str());
		std::stringstream ssbuffer;
		std::string buffer;

		ssbuffer << file.rdbuf();
		file.close();
		_response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
		buffer = ssbuffer.str();
		_response.append(SSTR("" << buffer.size()));
		_response.append("\n\n");
		_response.append(buffer);
	}
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
