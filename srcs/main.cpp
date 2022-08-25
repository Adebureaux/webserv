#include "Server.hpp"

int main(int ac, char **argv, char **envp)
{
	(void)ac;
	(void)argv;
	(void)envp;
	Server server("127.0.0.1", 8080);
	return (0);
}
