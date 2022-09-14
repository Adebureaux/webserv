#pragma once

#include <set>
#include <map>
#include <string>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <stdexcept>
#include <sys/epoll.h>
#include <arpa/inet.h>

// Aymeric
#define GET						 0
#define POST					 1
#define DELETE					 2
#define NO_METHOD				-1

#define OR						 1
#define AND						 0

#define STAR_NO_MIN				 0
#define STAR_NO_MAX				-1
#define EXECP					std::invalid_argument(std::string(__FUNCTION__) + "/"+ std::string(e.what()))
#define CATCH_HEADER_VAR(X)		_header_var_map[X] = std::string(_raw_request.begin()+ old_head,_raw_request.begin() + _head)
#define CATCH_VAR(X)			_var_map[X] = std::string(_raw_request.begin()+ old_head,_raw_request.begin() + _head)
// End Aymeric

// Augustin
typedef struct s_config {
	std::string	address;
	std::string	server_name;
	int			port;
} t_config;

#define BUFFER_SIZE 4096
#define MAX_EVENTS 128
#define TIMEOUT_VALUE 30000
// End Augustin

/*
** color letters
*/
# define C_BLACK 		"\033[30m"
# define C_RED 			"\033[31m"
# define C_GREEN		"\033[32m"
# define C_YELLOW		"\033[33m"
# define C_BLUE			"\033[34m"
# define C_MAGENTA		"\033[35m"
# define C_CYAN			"\033[36m"
# define C_WHITE		"\033[37m"
# define C_GRAY			"\033[90m"
/*
** color letters Gras
*/
# define C_G_BLACK 		"\033[30;01m"
# define C_G_RED 		"\033[31;01m"
# define C_G_GREEN		"\033[32;01m"
# define C_G_YELLOW		"\033[33;01m"
# define C_G_BLUE		"\033[34;01m"
# define C_G_MAGENTA	"\033[35;01m"
# define C_G_CYAN		"\033[36;01m"
# define C_G_WHITE		"\033[37;01m"
# define C_G_GRAY		"\033[90;01m"
/*
** color background
*/
# define C_B_BLACK 		"\033[40m"
# define C_B_RED 		"\033[41m"
# define C_B_GREEN		"\033[42m"
# define C_B_YELLOW		"\033[43m"
# define C_B_BLUE		"\033[44m"
# define C_B_MAGENTA	"\033[45m"
# define C_B_CYAN		"\033[46m"
# define C_B_WHITE		"\033[47m"
# define C_B_GRAY		"\033[100m"
/*
** default color reset
*/
# define C_RES			"\033[0m"


/*
** DEBUG printf(TEST);
*/

// # define MINUS_STR	"----------------------------------------------------------------------------------------------------"
// # define TEST "\033[36;01m(%s) \033[31;01m%s\033[0m %.*s>\033[35;01m %d\033[0m\n", __FILE__, __FUNCTION__, (int)(50 - (strlen(__FUNCTION__) + strlen(__FILE__) + 3)), MINUS_STR, __LINE__
