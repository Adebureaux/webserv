#include "../includes/SimpleSocket.hpp"
#include "../includes/parsing.hpp"
#include "../includes/Response.hpp"

#define MAX_EVENTS 5

std::vector<Response> resp;

int compare_path(std::string root, std::string page)
{
    size_t test1;
    size_t test2;
    int res;
    
    res = 0;
    if (root.size() > 1)
        root = root.substr(1, root.size() - 1);
    else
        return (1);
    page = page.substr(1, page.size() - 1);
    test1 = root.find_first_of("/");
    test2 = page.find_first_of("/");
    while (test1 != root.npos && test2 != page.npos)
	{
        std::string check1 = root.substr(0, test1);
        std::string check2 = page.substr(0, test2);
        if (check1 != check2)
            return (res);
        res++;
        root = root.substr(test1 + 1, root.size() - test1 + 1);
        page = page.substr(test2 + 1, page.size() - test2 + 1);
        test1 = root.find_first_of("/");
        test2 = page.find_first_of("/");
    }
    return (res);
}

Response used_server_validation(Response resp, nginx server)
{
    if (server.conf.data()[resp.specs].size() > 1)
	{
        std::map<std::string,std::map<std::string,std::vector<std::string> > >::iterator it = server.conf.data()[resp.specs].begin();
        int better = 0;
        int i = 0;
        while (it != server.conf.data()[resp.specs].end())
		{
            if (it->first != "server")
			{
                std::string loc = it->first.substr(9, it->first.size() - 9);
                int test = compare_path(loc, resp.re.page);
                if (test > better)
                    resp.loc = it->first;
            }
            i++;
            it++;
        }
    }
    else
        resp.loc = 1;
    std::size_t j = resp.specs + 1;
    int better = compare_path(resp.loc.substr(9, resp.loc.size() - 9), resp.re.page);
    while (j < server.conf.size())
	{
        std::map<std::string,std::map<std::string,std::vector<std::string> > >::iterator it = server.conf.data()[j].begin();
        int i = 0;
        while (it != server.conf.data()[j].end())
		{
            if (it->first != "server")
			{
                std::string loc = it->first.substr(9, it->first.size() - 9);
                int test = compare_path(loc, resp.re.page);
                if (test > better)
				{
                    resp.loc = it->first;
                    resp.specs = j;
                }
            }
            i++;
            it++;
        }
        j++;
    }
    resp.absolut_path = server.conf.data()[resp.specs][resp.loc]["root"][0];

	int i = 0;
    int g = 0;
    std::string temp = resp.loc.substr(9, resp.loc.size() - 9);
    while (resp.re.page[i] == temp[g])
	{
        i++;
        g++;
    }
    if (i >= 1)
        i--;
    resp.absolut_path.append(resp.re.page.substr(i, resp.re.page.size() - i));

	size_t d = resp.re.page.find_last_of(".");
    if (d != resp.re.page.npos)
	{
        std::string ext = resp.re.page.substr(d, resp.re.page.size() - d);
        if (server.conf.data()[resp.specs][resp.loc]["cgi"].size() > 0)
		{
            if (ext == server.conf.data()[resp.specs][resp.loc]["cgi"][0])
                resp.re.is_cgi = true;
            else
                resp.re.is_cgi = false;
        }
        else
            resp.re.is_cgi = false;
    }
    if (server.conf.data()[resp.specs]["server"]["client_max_body_size"].size() > 0 && atoi(server.conf.data()[resp.specs]["server"]["client_max_body_size"][0].c_str()) > 0)
	{
        int g = atoi(server.conf.data()[resp.specs]["server"]["client_max_body_size"][0].c_str());
        {
            if (static_cast< int >(resp.re.body.size()) > g)
                resp.re.error_type = 413;
        }
    }
    if (server.conf.data()[resp.specs][resp.loc]["methods"].size() > 0)
	{

        size_t h = 0;
        int cont = 0;
        while (server.conf.data()[resp.specs][resp.loc]["methods"].size() > h)
		{
            if (server.conf.data()[resp.specs][resp.loc]["methods"][h] == resp.re.type)
                cont++;
            h++;
        }

        if (cont == 0)
            resp.re.error_type = 405;
    }
    return (resp);
}

int check_conn(int fd_1, SimpleSocket *serv, int nbr)
{
    int i;

    i = 0;
    while (i < nbr)
	{
		if (fd_1 == serv[i].getServerFd())
            return (i);
        i++;
    }
    return (-1);
}

int fd_in_queue(int fd, int queue)
{
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLRDHUP;
    event.data.fd = fd;
    event.data.u64 = event.data.u32;
    if (epoll_ctl(queue, EPOLL_CTL_ADD, fd, &event)) //https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
	{
        close(fd);
        return (-1);
    }
    return (0);
}

void poll_handling(int epoll_fd, const int fd, struct epoll_event *event, SimpleSocket *serv, nginx server)
{
    SimpleSocket sock[1];
    int j;
    
    j = check_conn(fd, serv, server.conf.size());
    sock[0]._server_fd = fd;

    if (j != -1)
    {
        unsigned int i = 0;
        while (i != resp.size() && resp[i].fd_listen != fd)
            i++;

        int clientSocket = sock[0].acceptSocket();
        SimpleSocket test[1];
        test[0]._server_fd = clientSocket;

        if (fcntl(test[0]._server_fd, F_SETFL, O_NONBLOCK) < 0) //renvoie une erreur quand < 0; (syst call)  https://man7.org/linux/man-pages/man2/fcntl.2.html
            return ;

        if (fd_in_queue(test[0]._server_fd, epoll_fd))
            return ;

        resp[i].fd_accept = test[0]._server_fd;
    }
    else if (event->events & EPOLLRDHUP) //detect peer shutdown
	{
        unsigned int i = 0;
        while (i != resp.size() && resp[i].fd_read != fd)
            i++;

        // resp[i].init_re();
        close(sock[0]._server_fd);
    }
    else if (event->events & EPOLLOUT) //The associated file is available for write operations
	{
        unsigned int i = 0;
        while (i != resp.size() && resp[i].fd_read != fd)
            i++;
        int ret = 0;
        if (resp[i].re.status_is_handled == false)
		{
            std::string content = get_response(resp[i], server);
            ret = send(sock[0]._server_fd, content.c_str(), content.size(), 0);
            if (ret == 0)
			{
                close(sock[0]._server_fd);
                exit(0);
            }
            if (ret == 0 || ret == -1)
                return ;
            close(sock[0]._server_fd);
        }
    }
    else if (event->events & EPOLLIN) //The associated file is available for read operations.
	{
        unsigned int i = 0;
        while (i < resp.size() && (resp[i].fd_accept != fd))
            i++;
        int ret = 0;
        char buffer[3000];
        bzero(buffer, sizeof(buffer));
        ret = recv(sock[0]._server_fd, buffer, sizeof(buffer), MSG_DONTWAIT); //MSG_DONTWAIT Enables nonblocking operation; if the operation would block, the call fails
        if (ret == -1)
            return ;
        go_request(buffer, &(resp[i].re));
        resp[i] = used_server_validation(resp[i], server);
        if (resp[i].re.status_is_finished == true)
        {
            event->events = EPOLLOUT;
            epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock[0]._server_fd, event); //https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
            resp[i].fd_read = sock[0]._server_fd;
        }
    }
    else
        return ;
    return ;
}

int launch(nginx server)
{
    struct epoll_event event;  //https://man7.org/linux/man-pages/man7/epoll.7.html
    struct epoll_event events[MAX_EVENTS]; // number of fd attended at same time, here 5
    unsigned int nb_events;
    const char *check = NULL;
    std::string home = "127.0.0.1";

    int queue = epoll_create1(EPOLL_CLOEXEC);//returns a file descriptor referring to the new epoll instance.  This file descriptor is used for all the subsequent calls to the epoll interface.

    if (queue == -1)
	{
        std::cout << "Error: Couldn't create epoll queue handler..." << std::endl;
        return (1);
    }

    SimpleSocket serv[server.conf.size()];

    size_t i = 0;
    while (i < server.conf.size())
	{
        size_t j = 0;
        while (j < server.conf.data()[i]["server"]["listen"].size())
		{
				int port = atoi(server.conf.data()[i]["server"]["listen"][j].c_str());
				check = serv[i].create_socket(port); //returns an error msg if it fails
				if (check == NULL)
				{

					if ((check = serv[i].identifySocket()) != NULL) //returns an error msg if it fails
					{
						std::cout << check << std::endl;
						return (1);
					}

					if (fcntl(serv[i].getServerFd(), F_SETFL, O_NONBLOCK) < 0)
					{
						std::cout << "Error : failed to set non blocking socket..." << std::endl;
						return (1);
					}

					if ((check = serv[i].listenSocket()) != NULL) //returns an error msg if it fails
					{
						std::cout << check << std::endl;
						return (1);
					}

					event.events = EPOLLIN;
					event.data.fd = serv[i].getServerFd();
                    event.data.u64 = event.data.u32;
					if (epoll_ctl(queue, EPOLL_CTL_ADD, serv[i].getServerFd(), &event)) // modifies queue by performing EPOLL_CTL_ADD on serv[i].getServerFd(). &event is the object linked to the file descriptor serv[i].getServerFd() https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
					{
						close(serv[i].getServerFd());
						std::cout << "Failed to add file descriptor to epoll\n" << std::endl;
						return (-1);
					}

					Response thread;
					thread.init_re();
					thread.fd_listen = serv[i].getServerFd();
					thread.specs = i;
					resp.push_back(thread);
				}
			j++;
		}
		i++;
    }
    while (1) //MAIN LOOP
	{
        nb_events = epoll_wait(queue, events, MAX_EVENTS, -1);

        i = 0;
        while (i < nb_events)
		{
            poll_handling(queue, events[i].data.fd, &events[i], serv, server);
            i++;
        }
    }

    close(queue);

    return (0);
}

void signalHandler(int signum) 
{
   std::cout << "Webserv status : Close.\nBye!" << std::endl;
   exit(signum);  
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Error : please add a .conf file as an argument..." << std::endl;
        return (1);
    }

    signal(SIGINT, signalHandler);

    nginx server = parser(argv[1]);

    if (!server.parsing_ok)
	{
        std::cout << "Error : invalid .conf file..." << std::endl;
        return (1);
    }

	(void)argv;

    return (launch(server));
}
