#include "Server.hpp"

int main(int ac, char **argv, char **envp)
{
	(void)ac;
	(void)argv;
	(void)envp;
	Server server;

	server.launch();
	return (0);
}
