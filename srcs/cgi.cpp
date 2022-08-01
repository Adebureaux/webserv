#include "../includes/cgi.hpp"

std::map<std::string, std::string> create_env(Response resp, nginx server)
{
    std::map<std::string, std::string> cgi;

    cgi.insert(std::make_pair("AUTH_TYPE", ""));
    cgi.insert(std::make_pair("REMOTE_HOST", ""));
    cgi.insert(std::make_pair("REMOTE_IDENT", ""));
    cgi.insert(std::make_pair("REMOTE_USER", ""));
    cgi.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
    cgi.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
    cgi.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.1"));
    cgi.insert(std::make_pair("REMOTE_ADDR", "127.0.0.1"));
    cgi.insert(std::make_pair("CONTENT_TYPE", resp.re.content_type));
    cgi.insert(std::make_pair("REQUEST_METHOD", resp.re.type));
    cgi.insert(std::make_pair("PATH_INFO", resp.re.page));
    cgi.insert(std::make_pair("PATH_TRANSLATED", resp.re.page));
    cgi.insert(std::make_pair("QUERY_STRING", resp.re.query));

    if (resp.re.query != "")
    {
        int j = std::count(resp.re.query.c_str(), resp.re.query.c_str() + resp.re.query.size(), '&');
        int k = -1;
        std::string content = resp.re.query;
        while (k < j)
        {
            std::string key = content.substr(0, content.find("="));
            size_t stop = content.find_first_of(" \n&\r");
            if (stop == content.npos)
            {
                std::string value = content.substr(key.size() + 1, content.size() - (key.size() + 1));
                cgi.insert(std::make_pair(key, value));
                break;
            }
            std::string value = content.substr(key.size() + 1, stop - (key.size() + 1));
            cgi.insert(std::make_pair(key, value));
            content = content.substr(stop + 1, content.size() - (stop + 1));
            k++;
        }
    }
    cgi.insert(std::make_pair("SCRIPT_NAME", resp.absolut_path));
    cgi.insert(std::make_pair("SERVER_NAME", server.conf.data()[resp.specs]["server"]["server_name"][0]));
    cgi.insert(std::make_pair("SERVER_PORT", server.conf.data()[resp.specs]["server"]["listen"][0]));

    if (resp.re.type == "GET")
        cgi.insert(std::make_pair("CONTENT_LENGTH", "0"));
    else 
    {
        int t = static_cast <int>(resp.re.body.size());
        std::stringstream ss;
        ss << t;
        std::string s = ss.str();
        cgi.insert(std::make_pair("CONTENT_LENGTH", s));
        cgi.insert(std::make_pair("BODY", resp.re.body));
    }
    return (cgi);
}

char **go_env(std::map<std::string, std::string> cgi)
{
    char **env = new char*[cgi.size() + 1];
    std::map<std::string, std::string>::iterator it = cgi.begin();
    unsigned int i;

    i = 0;
    while (it != cgi.end())
    {
        std::string tmp = it->first;
        tmp.append("=");
        tmp.append(it->second);
        env[i] = strdup(tmp.c_str());
        tmp.clear();
        it++;
        i++;
    }

    env[i] = NULL;
    return (env);
}

std::string handle_cgi(Response resp, nginx server)
{
    if (resp.re.type == "DELETE")
    {
        resp.re.error_type = 405;
        return (go_error(resp.re.error_type, server, resp));
    }

    resp.re.page = resp.re.page.substr(1, resp.re.page.size() - 1);
    resp.re.page = resp.re.page.substr(resp.re.page.find_last_of("/") + 1, resp.re.page.size() - resp.re.page.find_last_of("/") + 1);

    std::map<std::string, std::string> cgi = create_env(resp, server);
    std::string name = "cgi_output";
    std::string te = resp.absolut_path.substr(2, resp.absolut_path.find_last_of("/") - 1);

    int pipe_fd[2];
    int fd_save[2];
    pid_t pid;

    fd_save[0] = dup(STDIN_FILENO);
    fd_save[1] = dup(STDOUT_FILENO);
    
    if (pipe(pipe_fd))
        exit(EXIT_FAILURE);
    pid = fork();
    if (pid == -1)
        return "";
	else if (pid == 0)
    {
        char *args[2];
        char **env = go_env(cgi);
        chdir(te.c_str());
		args[0] = (char*)resp.re.page.c_str();
		args[1] = NULL;

		close(pipe_fd[1]);
		dup2(pipe_fd[0], 0);
		int	fd_tmp = open(name.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (fd_tmp < 0)
			return ("");
		dup2(fd_tmp, 1);
		if (execve(args[0], args, env) == -1)
        {
            // std::cerr << "sortie2     :   " << args[0] << "  |   " << args[1] << "   |   " <<  errno << std::endl;
            delete [] env;
            close(0);
		    close(fd_tmp);
		    close(pipe_fd[0]);
			return (go_error(500, server, resp));
        }
		close(0);
		close(fd_tmp);
		close(pipe_fd[0]);
        delete [] env;
		exit(0);
	}
	else
    {
		close(pipe_fd[0]);
		if (write(pipe_fd[1], resp.re.body.c_str(), resp.re.body.length()) < 0)
			return (go_error(500, server, resp));
		close(pipe_fd[1]);
		waitpid(pid, NULL, 0);
	}
	dup2(fd_save[0], STDIN_FILENO);
	dup2(fd_save[1], STDOUT_FILENO);
	close(fd_save[0]);
	close(fd_save[1]);
	if (pid == 0)
		exit(0);
    std::string de = te + "/cgi_output";
    std::basic_ifstream<char> fin(de.c_str());
	std::ostringstream oss;
	oss << fin.rdbuf();
	std::string ret(oss.str());
    std::string end = "HTTP/1.1 200 OK \r\nContent-Type: text/html Content-Length: " ;
    std::stringstream ss;
    ss << ret.size() + 23;
    end.append(ss.str());
    end.append("\r\n\r\n<html><body>" + ret + "</body></html>");
	return (end);
}
