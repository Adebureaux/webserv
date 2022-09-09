#include <cstdarg>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Request.hpp"

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
   // std::cout << "end" << std::endl;
   std::cout << std::endl;
   std::cout << "re.get_raw_request() :"<< std::endl << re.get_raw_request() << std::endl;
   std::cout << "re.get_connection() :'" << re.get_connection() << "'"<< std::endl;
   std::cout << "re.get_authority() :'" << re.get_authority() << "'"<< std::endl;
   std::cout << "re.get_host() :'" << re.get_host() << "'" << std::endl;
   std::cout << "re.get_request_target() :'" << re.get_request_target()<< "'" << std::endl;
   // std::cout << re.get_message_body() << std::endl;   
}