#include "Response.hpp"
#include  <sys/wait.h>

#define _MAX_ITERATIONS 5000

static std::string readToString(int fd)
{
	static char buffer[BUFFER_SIZE];
	std::stringstream res;
	int ret = 0;
	while ((ret = read(fd, buffer, sizeof(buffer))) != 0)
	{
		res << buffer;
		std::memset(buffer, 0, ret);
	}
	close(fd);
	return res.str();
}

template<class T>
const std::string itos(T number)
{
	std::stringstream stream;
	stream << number;
    return stream.str();
}
// name=asdasd&email=rballage%40student.42.fr&website=asdasd.com&comment=asdasdasdas&gender=male&submit=Submit

void Response::_cgi(const Message &request, Server_block& config)
{
	int out[2], error[2];
	int pid, status;
	if (access(_location->CGI.c_str(), F_OK) || access(_location->CGI.c_str(), X_OK))
	{
		_isCGI = false;
		_construct_error(500, true); 
	}

	FILE	    *temp = tmpfile();
	long	    fd_temp = fileno(temp);
	// FILE	    *temp_error = tmpfile();
	// long	    fd_temp_error = fileno(temp_error);
	std::vector<std::string> vec;
	std::string body = std::string(request.raw_data.begin() + request.header_size, request.raw_data.end());
	// std::cout << "CGI POST:\n" << request.raw_data << std::endl;
	vec.reserve(18);
	vec.push_back(std::string("SERVER_SOFTWARE=webserv/1.0"));
	vec.push_back(std::string(std::string("SERVER_NAME=") + config.server_names));
	vec.push_back(std::string(std::string("SERVER_PORT=") + itos(config.port)));
	vec.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	vec.push_back(std::string("GATEWAY_INTERFACE=PHP/7.4.3"));
	vec.push_back(std::string(std::string("REQUEST_METHOD=") + request.info.get_var_by_name("METHOD").second));
	vec.push_back(std::string(std::string("QUERY_STRING=") + request.info.get_var_by_name("QUERY_STRING").second));
	vec.push_back(std::string("REDIRECT_STATUS=200"));
	vec.push_back(std::string(std::string("HTTP_REFERER=") + request.info.get_header_var_by_name("Http-referer").second));
	vec.push_back(std::string(std::string("HTTP_USER_AGENT=") + request.info.get_header_var_by_name("User-Agent").second));
	vec.push_back(std::string(std::string("HTTP_ACCEPT=") + request.info.get_header_var_by_name("Accept").second));
	vec.push_back(std::string(std::string("HTTP_ACCEPT_LANGUAGE=") + request.info.get_header_var_by_name("Accept-Language").second));
	vec.push_back(std::string(std::string("HTTP_ACCEPT_ENCODING=") + request.info.get_header_var_by_name("Accept-Encoding").second));
	vec.push_back(std::string(std::string("SCRIPT_FILENAME=") + _file.uri));
	// vec.push_back(std::string("REMOTE_HOST="));
	vec.push_back(std::string(std::string("HTTP_COOKIE=") + request.info.get_header_var_by_name("Accept-Cookie").second));
	if (request.info.get_method() == POST)
	{
		// vec.push_back(std::string(std::string("PATH_INFO=") + _location->upload.second));
		vec.push_back(std::string(std::string("CONTENT_LENGTH=") + itos(request.indicated_content_size)));
		vec.push_back(std::string(std::string("CONTENT_TYPE=") + request.info.get_header_var_by_name("Content-Type").second));
	}
	std::vector<char *> cvec;
	cvec.reserve(vec.size());
    for(size_t i = 0; i < vec.size(); i++)
    {
	    cvec.push_back(const_cast<char*>(vec[i].c_str()));
		// std::cout << __FUNCTION__ <<"  " << vec[i].c_str() <<std::endl;
	}
	cvec.push_back(NULL);
	if (request.info.get_method() == POST)
	{
		write(fd_temp, body.c_str(), request.indicated_content_size);
		lseek(fd_temp, 0, SEEK_SET);
	}
	pipe(out);// non, fichier
	pipe(error); // non, fichier
	if ((pid = fork()) == -1)
		throw std::exception();
	if (pid == 0)
	{
		std::vector<char *> cvec2;
		cvec2.reserve(4);
		cvec2.push_back(const_cast<char*>(_location->CGI.c_str()));
		cvec2.push_back(const_cast<char*>(_file.uri.c_str()));
		cvec2.push_back(NULL);
		close(error[0]);
		close(1);
		close(out[0]);
		dup(out[1]);
		dup2(fd_temp, 0);
		dup2(error[1], 2);
		close(out[1]);
		close(error[1]);

		execve(cvec2[0], &cvec2[0], &cvec[0]);
		exit(-1);
	}

	pid_t ret = pid;
	int wait_iterations = 0;
	do {
		if ((ret = waitpid(ret, &status, WNOHANG)) == -1)
			std::cerr << ("wait() error") << std::endl;
		else if (ret == 0)
		{
			wait_iterations++;
			std::cout << "waiting - " << wait_iterations << "\n";
			usleep(100);
		}
		else 
		{
			if (WIFEXITED(status))
			{
				std::cout << "child exited with status code:" << WEXITSTATUS(status) << std::endl;
				break;
			}
			else std::cout << "child exited did not exited successfuly with status code:" << WEXITSTATUS(status) << std::endl;
		}
	} while (ret == 0 && wait_iterations < _MAX_ITERATIONS);
	
	if (wait_iterations >= _MAX_ITERATIONS)
	{
		status = -1;
		kill(pid, SIGKILL);
	}
	LOG
	close(out[1]);
	close(error[1]);
	std::string cgi_out = readToString(out[0]);
	std::string cgi_out_error = readToString(error[0]);
	close(out[0]);
	close(error[0]);
	fclose(temp);
	if (cgi_out_error.empty() and !cgi_out.empty() and status == 0)
	{
		_file.content = cgi_out;
		_construct_response(request, 200);
	}
	else if (!cgi_out_error.empty() and !cgi_out.empty() && status == 0)
	{
		_isCGI = false;
		_construct_error(400, true);
	}
	else
	{
		_isCGI = false;
		_construct_error(500, true);
	}
}
