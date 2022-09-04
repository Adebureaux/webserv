#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
# include <string>
#include <stdio.h>
#include <unistd.h>
# include <sstream>

# include <iostream>



// #define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define CLEAR "\033[0m"

int main(int ac, char *const * av) {
	int out[2], error[2], pid;
	(void)ac;
	av++;
	pipe(out);
	pipe(error);
	if ((pid = fork()) == -1)
		std::cout << "error: pipe() || fork()\n";
	if (pid == 0)
	{
		close(out[0]);
		close(error[0]);
		dup2(out[1], 1);
		dup2(error[1], 2);  // send stderr to the pipe
		close(out[1]);
		close(error[1]);
		execl("/usr/bin/php-cgi", "/usr/bin/php-cgi", *av, 0);
		exit(1);
	}
	int status = 0;
	waitpid(pid, &status, 0);
	static char buffer_out[2048];
	static char buffer_error[2048];
	close(out[1]);
	close(error[1]);
	while (read(out[0], buffer_out, sizeof(buffer_out)) != 0) {}
	close(out[0]);
	while (read(error[0], buffer_error, sizeof(buffer_error)) != 0) {}
	close(error[0]);
	std::cout<< GREEN <<"CGI OUT:\n" << buffer_out << CLEAR;
	std::cout<<std::endl << RED <<"CGI ERROR:" << status <<"\n" << buffer_error << CLEAR;
	return 0;
}
