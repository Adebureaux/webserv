#include <cstdarg>
#include <iostream>
#include <string>
#include "Request.hpp"

int main(void)
{
   std::string req("POST / HTTP/1.1\r\nConnection: \n\rKeep-Alive\r\nHost: localhost:1234\r\n\n\r");
   Request  re(req);
   std::cout << "end" << std::endl;
}