#include "SimpleSocket.hpp"
#include "Request.hpp"
#include "Response.hpp"

SimpleSocket::SimpleSocket(void)
{
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror_exit("cannot create socket");
	int enable = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		perror_exit("cannot set socket option 'SO_REUSEADDR'");
	identify();
	listenSocket();
	while (1)
	{
		acceptSocket();
		communicate();
	}
}

SimpleSocket::~SimpleSocket(void)
{
	close(_server_fd);
	close(_socket_fd);
	close(_epoll_fd);
}

void SimpleSocket::identify(void)
{
	_port = 8080; // Where the clients can reach at
	/* htonl converts a long integer (e.g. address) to a network representation */ 
	/* htons converts a short integer (e.g. port) to a network representation */ 
	memset((char*)&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1) 
		perror_exit("bind failed"); 
}

void SimpleSocket::listenSocket(void)
{
	if (listen(_server_fd, 1) == -1)
		perror_exit("listen failed");
}

void SimpleSocket::acceptSocket(void)
{
	unsigned int addrlen = sizeof(_address);
	if ((_socket_fd = accept(_server_fd, (struct sockaddr *)&_address, (socklen_t*)&addrlen)) == -1)
		perror_exit("accept failed");
}

void SimpleSocket::communicate(void)
{
	Bundle_for_response bd;
	bd.init_re();

	char buffer[1024];
	int valread = read(_socket_fd, buffer, 1024);
	if (valread == -1)
		exit(1);
	buffer[valread] = '\0';
	//std::cout << buffer << std::endl;
	// GET FILE NAME HERE

	first_dispatch(buffer, &bd.re);	// request parsing

	std::cout << "request done." << std::endl << std::endl;

	std::string res = get_response(bd);	// response

	std::cout << "response done." << std::endl << std::endl;

	std::cout << "reeeeeeeees = |\n" << res << "\n| reeeeeeeeees" << std::endl << std::endl;	//////// a effacer //////

	// OPEN index.html as html page

	int ret = send(_socket_fd, res.c_str(), res.size(), 0);

	std::cout << "ret = " << ret << std::endl;

	// write(_socket_fd, res.c_str(), res.size());
}

void SimpleSocket::perror_exit(std::string err)
{
	std::cerr << err << " : " << strerror(errno) << std::endl;
	this->~SimpleSocket();
	exit(errno);
}
