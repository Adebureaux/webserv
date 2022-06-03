#ifndef PARSING_CONF_H
#define PARSING_CONF_H

#include <string>
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

class config
{
	public :

	int nbrSrv;
	std::multimap<int, std::multimap<std::string, std::string> > server; //int : index du serveur. dans ce serveur : string 1 = "listen" par ex, et string 2 = "5000"...

	

};

config parser(char *file_name);

#endif