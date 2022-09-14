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
      (void)av;
	  // req = "POST / HTTP/1.1\r\nConnection: blalba\r\nHost: localhost:1234\r\n\r\n";
	//   req = "GET /index.php?truc=machin&foo=bar&jean-michel=francis HTTP/1.1\r\nUser-Agent: custom-client\r\nHost: weebserv\r\nAccept-Language: en-US\r\nConnection: Keep-Alive\r\nContent-type: text/html\r\nContent-length: 16\r\n\r\n";
     req = "GET / HTTP/1.1\r\nHost: localhost:8080 \r\nConnection: keep-alive \r\nCache-Control: max-age=0 \r\nsec-ch-ua: \"Google Chrome\";v=\"105\", \"Not)A;Brand\";v=\"8\", \"Chromium\";v=\"105\" \r\nsec-ch-ua-mobile: ?0 \r\nsec-ch-ua-platform: \"Linux\"Upgrade-Insecure-Requests: 1 \r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36 \r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9 \r\nSec-Fetch-Site: none \r\nSec-Fetch-Mode: navigate \r\nSec-Fetch-User: ?1 \r\nSec-Fetch-Dest: document \r\nAccept-Encoding: gzip, deflate, br \r\nAccept-Language: en-US,en;q=0.9,fr;q=0.8 \r\n\r\n";
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
