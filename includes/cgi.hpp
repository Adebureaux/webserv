#include "parsing.hpp"
#include "Request.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <istream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>

std::string handle_cgi(Response resp, nginx server);
std::string go_error(int err, nginx server, Response resp);