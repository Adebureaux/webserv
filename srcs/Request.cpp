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
	std::cout << msg << std::endl;		///////// a effacer /////////

	if (r->encoding == "chunked" && r->pure_content != "")
		chunked_post(msg, r);
}