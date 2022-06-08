#ifndef PARSING_CONF_H
#define PARSING_CONF_H

#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <map>
#include <cctype>
#include <cstring>
#include <utility>

#define TRUE 1
#define FALSE 0


	int 															nbrSrv;
	std::string 													file;
	std::multimap<std::string, std::string>							bloc;//ce que je vais mettre dans 'servers', à la suite de l'int indiquant l'index du bloc de serveur actuel
	std::multimap<int, std::multimap<std::string, std::string> > 	servers; //int : index du serveur. dans ce serveur : string 1 = "listen" par ex, et string 2 = "5000"...


	int parser(char *file_name);
	int get_nbr_serv(std::string file_name);
	int check_brackets(std::string content);
	std::string get_file_content(std::string file_name);




#endif