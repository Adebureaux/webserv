#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "../includes/Request.hpp"

std::string get_response(Bundle_for_response bfr);

#endif