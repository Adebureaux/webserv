#include <cstdarg>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Request.hpp"
#include "color_shell.hpp"

// "POST / HTTP/1.1rnConnection: Keep-AlivernHost: localhost:1234\r\n\r\n"

int main(int ac, char **av)
{
   std::string req;
   if (ac == 2)
   {
	  std::ifstream file(av[1]);
	  std::stringstream iss;
	  iss << file.rdbuf();
	  req = iss.str();
	  // file >> req;
   }
   else
   {
	  // req = "POST / HTTP/1.1\r\nConnection: blalba\r\nHost: localhost:1234\r\n\r\n";
	  req = "GET /index.php?truc=machin&foo=bar&jean-michel=francis HTTP/1.1\r\nUser-Agent: custom-client\r\nHost: weebserv\r\nAccept-Language: en-US\r\nConnection: Keep-Alive\r\nContent-type: text/html\r\nContent-length: 16\r\n\r\n";
   }
   Request  re(req);
   std::cout << "req : ";
   if (re.is_valid())
      std::cout << C_G_GREEN << "OK";
   else
      std::cout << C_G_RED << "KO";
   std::cout << C_RES << std::endl << std::endl;
   std::cout << "re.get_raw_request() :"<< std::endl << re.get_raw_request() << std::endl;
   std::map<std::string, std::string> req_map = re.get_var_map();
   std::map<std::string, std::string>::iterator it = req_map.begin();
   std::map<std::string, std::string>::iterator ite = req_map.end();
   for(;it != ite; it++)
   {
      std::cout << it->first << " : \"" << it->second << "\"" << std::endl;
   }

   std::cout << std::endl;
   std::map<std::string, std::string> req_head_map = re.get_header_var_map();
   std::map<std::string, std::string>::iterator it_head = req_head_map.begin();
   std::map<std::string, std::string>::iterator ite_head = req_head_map.end();
   for(;it_head != ite_head; it_head++)
   {
      std::cout << it_head->first << " : \"" << it_head->second << "\"" << std::endl;
   }
}