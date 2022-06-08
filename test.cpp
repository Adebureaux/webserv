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

int main ()
{
	std::multimap<std::string, std::string>							bloc;
	std::multimap<std::string, std::string>::iterator 				iti;

	bloc.insert(std::pair<std::string, std::string>("hello", "world"));
	bloc.insert(std::pair<std::string, std::string>("ca", "roule"));
	iti=bloc.begin();
	std::cout << (*iti).first << " ==> " << (*iti).second << std::endl;
	++iti;
	std::cout << (*iti).first << " ==> " << (*iti).second << std::endl;
	
	

}