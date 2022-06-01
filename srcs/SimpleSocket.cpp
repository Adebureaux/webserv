#include "SimpleSocket.hpp"

SimpleSocket::SimpleSocket()
{
	/* Create socket */
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(_socket == -1)
	{
		perror("socket()");
		exit(errno);
	}

	/* Create interface */
	struct sockaddr_in sin = { 0 };

	sin.sin_addr.s_addr = htonl(INADDR_ANY); /* nous sommes un serveur, nous acceptons n'importe quelle adresse */

	sin.sin_family = AF_INET;

	sin.sin_port = htons(PORT);

	if(bind (sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
	{
		perror("bind()");
		exit(errno);
	}
}

SimpleSocket::~SimpleSocket() {}
