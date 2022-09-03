#include <cstdarg>
#include <iostream>
#include <string>
#include "Request.hpp"

// "POST / HTTP/1.1rnConnection: Keep-AlivernHost: localhost:1234\r\n\r\n"

int main(void)
{
   std::string req("POST / HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: localhost:1234\r\n\r\n");
   Request  re(req);
   std::cout << "end" << std::endl;
}