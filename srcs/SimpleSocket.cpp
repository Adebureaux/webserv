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
	struct sockaddr_in sin;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8080);
	if (bind(_socket, (sockaddr*)&sin, sizeof(sin)) == -1)
	{
		perror("bind()");
		exit(errno);
	}

	/* Listening client connexion */
	if (listen(_socket, 5) == -1)
	{
		perror("listen()");
		exit(errno);
	}
	struct sockaddr_in csin;
	unsigned int size =  sizeof(csin);
	_csocket = accept(_socket, (sockaddr*)&csin, &size);
	if (_csocket == -1)
	{
		perror("accept()");
    	exit(errno);
	}
}

SimpleSocket::~SimpleSocket() {}
