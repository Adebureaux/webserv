#ifndef RESPONSE_HPP
# define RESPONSE_HPP


#include "SimpleSocket.hpp"
#include "parsing.hpp"
#include "Request.hpp"
#include "cgi.hpp"

#include <algorithm>
#include <sys/types.h> 
#include <sys/epoll.h> //queue management, fd multithread
#include <vector>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <sstream>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>



std::string get_response(Response resp, nginx server);

#endif