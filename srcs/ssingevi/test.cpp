#include <cstdarg>
#include <iostream>
#include <string>
#include "Request.hpp"

// "POST / HTTP/1.1rnConnection: Keep-AlivernHost: localhost:1234\r\n\r\n"

int main(void)
{
   std::string req("POST / HTTP/1.1\r\nConnection: blalba\r\nHost: localhost:1234\r\n\r\n");
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