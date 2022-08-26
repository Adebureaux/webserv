#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int result;
	fd_set readfds, testfds;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8083);
	server_len = sizeof(server_address);

	int opt = 1;
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	listen(server_sockfd, 5);

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);

	while (1) {
		char ch;
		int fd;
		int nread;

		testfds = readfds;

		printf("server waiting\n");
		result = select(FD_SETSIZE, &testfds, (fd_set *)0,
				        (fd_set *)0, (struct timeval *)0);

		std::cout << "ret select = " << result << std::endl;

		if (result < 1) {
			perror("server5");
			exit(1);
		}

		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				std::cout << "ENTER WITH FD = " << fd << std::endl;
				if (fd == server_sockfd) {
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd,
							(struct sockaddr *)&client_address, (socklen_t*)&client_len);
					FD_SET(client_sockfd, &readfds);
					printf("add client on fd %d\n", client_sockfd);
				} else {
					ioctl(fd, FIONREAD, &nread);

					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
					} else {
						char buffer[1024];

						int rd = recv(fd, buffer, 1024, 0);
						buffer[rd] = '\0';
						std::cerr << buffer << std::endl;
						printf("serving client on fd %d\n", fd);
						send(fd, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 13\n\nHello World!", 73, 0);
					}
				}
			}
		}
	}
}
