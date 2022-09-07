#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
# include <string>
#include <stdio.h>
#include <unistd.h>
# include <sstream>
# include <iostream>
#include <cstring>


// #define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define CLEAR "\033[0m"

void set_cgi_env(void)
{
	setenv("SERVER_SOFTWARE", "webserv/1.0", 1); // Le nom et la version du serveur HTTP répondant à la requête. (Format : nom/version)
	setenv("SERVER_NAME", "localhost", 1); // Le nom d'hôte, alias DNS ou adresse IP du serveur.
	setenv("GATEWAY_INTERFACE", "PHP/7.4.3", 1); // La révision de la spécification CGI que le serveur utilise. (Format : CGI/révision)
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1); // Le nom et la révision du protocole dans lequel la requête a été faite (Format : protocole/révision)
	setenv("SERVER_PORT", "8080", 1); // Le numéro de port sur lequel la requête a été envoyée.
	setenv("REQUEST_METHOD", "GET", 1); // La méthode utilisée pour faire la requête. Pour HTTP, elle contient généralement « GET » ou « POST ».
	setenv("QUERY_STRING", "name=Romain", 1); // Contient tout ce qui suit le « ? » dans l'URL envoyée par le client. Toutes les variables provenant d'un formulaire envoyé avec la méthode « GET » seront contenues dans le QUERY_STRING sous la forme « var1=val1&var2=val2&... ».
	setenv("PATH_TRANSLATED", "index.php", 1); // Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.
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

std::string readToString(int fd)
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

void cgi(const char *script_path) {
	int out[2], error[2], pid, status;

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
		// exit(1);
	}
	waitpid(pid, &status, 0);
	close(out[1]);
	close(error[1]);
	std::cout<< GREEN << "CGI OUT:\n" << readToString(out[0]) << CLEAR;
	std::cout<<std::endl << RED << "CGI ERROR:" << status << "\n" << readToString(error[0]) << CLEAR;
}

int main(int ac, char *const * av) {
	int out[2], error[2], pid;
	(void)ac;
	av++;
	cgi(*av);
	return 0;
}
