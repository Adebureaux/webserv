#ifndef PARSING_H
# define PARSING_H

#include <cctype>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <string>
#include <iostream>
#include <cstdlib>
#include <map>

#define TRUE 1
#define FALSE 0

class nginx
{

	public :

		int 																						nbrSrv; //number of servers
		bool																						parsing_ok; //1 == valid; 0 == error
		std::string 																				file; //the whole conf file
		std::map< std::string, std::vector< std::string > >											innerBloc; //a server inner bloc, wether server classic infos or one of the locations blocs
		std::map< std::string, std::map< std::string, std::vector <std::string > > > 				servBloc; // a whole server bloc... we push innerBloc(s) in it 1 by 1 while parsing content
		std::vector< std::map< std::string, std::map< std::string, std::vector <std::string > > > > conf; //the whole conf file in a 4 dimensions data structure. [0] : a whole server bloc, [1] : an inner bloc within a server (check above), [2] : a key that may contain more than 1 associated value [3] : value(s);



	int check_server_names();
	int check_only_different_ports();
	void get_elements(std::string content, int *index);
	int get_nbr_serv(std::string file_name);
	int check_brackets(std::string content);
	std::string get_file_content(std::string file_name);
	int check_file_name(std::string file_name);
	std::string getAServerValue(int serverIndex, std::string key);
	std::string getALocationValue(int serverIndex, std::string key);
	void printMap();

};//class nginx

nginx parser(char *file_name);



#endif