#include "Request.hpp"


std::string get_filename(std::string mess)
{
    int file_start = mess.find("filename= \"") + 11;

    if (file_start == 11)
        return "";

    std::string temp = mess.substr(file_start, mess.size() - file_start);
    std::string filename = temp.substr(0, temp.find_first_of("\""));

    return filename;
}

std::string get_query(std::string page)
{
    std::size_t test = page.find_first_of("?");

    if (test == page.npos)
        return "";

    return page.substr(test, page.size() - test);
}

void fill_request_basic(char *msg, int n, Request *r)
{
	int i;
	int j;

	if (n == 1){
		r->type = "GET";
		i = 4;
	}
	else if (n == 3){
		r->type = "POST";
		i = 5;
	}
	else {
		r->type = "DELETE";
		i = 7;
	}

	j = i;
	while (msg[j] != ' ')
		j++;
	std::string mess = msg;
	r->page = mess.substr(i, j - i);
	r->query = get_query(r->page);

	j += 1;
	i = j;
	while (msg[i] != ' ' && msg[i] != '\n' && msg[i] != '\r')
		i++;
	std::string http = mess.substr(j, i - j);
	if (http != "HTTP/1.1"){
		r->error_type = 505;
		return ;
	}

	std::size_t k = mess.find("Host: ");
	if (k == mess.npos){
		r->error_type = 400;
		return ;
	}

	size_t l = k + 7;
	while (mess[l] != ':')
		l++;
	r->host_address = mess.substr(k, l - k);

	l++;
	k = l;
	while (mess[l] != ' ' && mess[l] != '\n' && mess[l] != '\r')
		l++;

	std::string ip = mess.substr(k, l - k);
	r->host_ip = atoi(ip.c_str());
	r->error_type = 200;

	//std::cout << r->page << std::endl;		///////// a effacer /////////

	return ;
}

void chunked_post(std::string mess, Request *r)
{

    r->status_is_finished = false;		// begining of parsing so false
    r->filename = get_filename(mess);

    std::size_t deb;
    if (r->pure_content == "")
	{
        deb = mess.find("\r\n\r\n") + 4;
        r->body = mess.substr(deb, mess.size() - deb);	// take off previous request
    }
    else
	{
        r->body = mess;
        deb = 0;
    }

    std::string to_go = r->body;

    size_t j = 0;
    while (j < r->body.size())
	{
        deb = to_go.find_first_of('\r');
        std::string hex = to_go.substr(0, deb);
        int to_cover = (int)strtol(hex.c_str(), NULL, 16);
        if (to_cover == 0){
            r->status_is_finished = true;
            r->body = r->pure_content;
            return;
        }
        deb += hex.size() + 2;
        r->pure_content.append(to_go.substr(deb, to_cover));
        j += deb + to_cover;
        to_go = to_go.substr(deb + to_cover , to_go.size() - (deb + to_cover));
    }

}

void	first_dispatch(char *msg, Request *r)
{
	//std::cout << "yoooooooo\n" << msg << std::endl;		///////// a effacer /////////

	if (r->encoding == "chunked" && r->pure_content != "")
		chunked_post(msg, r);

	else if (msg[0] == 'G' && msg[1] == 'E' && msg[2] == 'T' && msg[3] == ' ')
	{
        fill_request_basic(msg, 1, r);
        r->status_is_finished = true;
    }
}