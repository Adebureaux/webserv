#include "../includes/Response.hpp"

////////////////////////////////////////////////////////////////
//                           UTILS                            //
////////////////////////////////////////////////////////////////

std::string findExtension(std::string filepath)
{
    size_t i;

    if ((i = filepath.rfind('.')) == std::string::npos)
        return "none";

    std::string ext = filepath.substr(i+1);
	
    if (ext == "html")
        return "text/" + ext + "\n";
    else if (ext == "cgi")
        return "test/html";
    else
        return "image/" + ext;
}

std::string go_redirect(Response resp, nginx server)
{
    std::string response = "HTTP/1.1 ";
    std::string to_extract = server.conf.data()[resp.specs][resp.loc]["return"][0];
    std::string t = to_extract.substr(0, to_extract.find_first_of(" "));
    response.append(t);
    response.append("\r\nLocation: ");
    response.append(to_extract.substr(to_extract.find_first_of(" ") + 1, to_extract.size() - to_extract.find_first_of(" ") + 1));
    return response;
}

std::string go_directory(Response resp, nginx server)
{
    std::string response = "";
    std::string body = "";
    std::string temp = resp.absolut_path.substr(resp.absolut_path.find_last_of("/") + 1, resp.absolut_path.size() - resp.absolut_path.find_last_of("/") + 1);
    struct stat s;
    if ( stat(temp.c_str(), &s) == 0 )
	{
        if ( s.st_mode & S_IFDIR )
		{
            if (resp.re.type == "DELETE")
                return go_error(403, server, resp);

            if (server.conf.data()[resp.specs][resp.loc]["autoindex"].size() > 0)
			{
                if (server.conf.data()[resp.specs][resp.loc]["autoindex"][0] == "1")
				{
                    struct dirent *dp;
                    DIR *dirp = opendir(temp.c_str());
                    while ((dp = readdir(dirp)) != NULL){
                        body.append(dp->d_name);
                        body.append("\n");
                    }
                    (void)closedir(dirp);
                    response.append(body);
                }
            }
            else
                return go_error(403, server, resp);
        }
    }
    else
	{
        // std::cout << errno << std::endl;
        return response;
    }
    return response;
    // TO DO handle if absolut path fucked
}

////////////////////////////////////////////////////////////////
//                          ERROR                             //
////////////////////////////////////////////////////////////////

std::string set_error(int err)
{
    std::string error_msg = "";
    std::string response = "HTTP/1.1 ";
    if (err == 400)
	{
// for now for missing extension in file
        error_msg = "Bad Request";
        response.append("400 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 101\n\n");
        response.append("<html><body>400 BAD REQUEST<img src=\"error/400.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    if (err == 403)
	{
// for now for missing extension in file
        error_msg = "Forbidden";
        response.append("403 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 99\n\n");
        response.append("<html><body>403 FORBIDDEN<img src=\"error/403.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    else if (err == 404)
	{
// for now, couldn't open file so does not exist
        error_msg = "Not Found";
        response.append("404 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 91\n\n");
        response.append("<html><body>404 NOT FOUND<img src=\"error/404.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    else if (err == 405)
	{
// is not GET POST or DELETE
        error_msg = "Method Not Allowed";
        response.append("405 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 108\n\n");
        response.append("<html><body>405 METHOD NOT ALLOWED<img src=\"error/405.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    else if (err == 411)
	{
// content lenght missing
        error_msg = "Length Required";
        response.append("411 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 106\n\n");
        response.append("<html><body>411 LENGTH REQUIRED <img src=\"error/411.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    else if (err == 413)
	{
// content lenght missing
        error_msg = "Playload Too Large";
        response.append("413 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 108\n\n");
        response.append("<html><body>413 PLAYLOAD TOO LARGE<img src=\"error/413.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    else if (err == 500)
	{
// For now, couldn't delete file
        error_msg = "Internal Server Error";
        response.append("500 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 111\n\n");
        response.append("<html><body>500 INTERNAL SERVER ERROR<img src=\"error/500.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    else if (err == 505)
	{
// bad hhtp protocol version
        error_msg = "HTTP Version not supported";
        response.append("505 ");
        response.append(error_msg);
        response.append(" Content-Type: text/html Context-Lenght: 116\n\n");
        response.append("<html><body>505 HTTP VERSION NOT SUPPORTED<img src=\"error/505.jpg\" alt=\"\" width=\"600\" height=\"750\"> </body></html>");
    }
    return response;
}

std::string go_error(int err, nginx server, Response resp)
{

// EN PLUS NOW error 413 PAYLOAD TOO LARGE
// 403 FORBIDDEN


    std::string response = "HTTP/1.1 ";
        //DO smthing
        // si error page presente
        // 404 NOT FOUND -> code d'erreur et explication
        // Content-Type: text/html Context-Lenght: 109\r\n\r\n -> type de page et taille fichier
        // ->fichier
    size_t i = 0;
    size_t j = 0;
    std::string url = "";
    if (server.conf.data()[resp.specs][resp.loc]["root"][0].size())
        url += server.conf.data()[resp.specs][resp.loc]["root"][0];
    while (i < server.conf.data()[resp.specs]["server"]["error_page"].size())
    {
        if (server.conf.data()[resp.specs]["server"]["error_page"][i].length() >= 3 && atoi(server.conf.data()[resp.specs]["server"]["error_page"][i].substr(0, 3).c_str()) == err \
        && server.conf.data()[resp.specs]["server"]["error_page"][i].find_first_not_of("\t\n\r\v\f ", 3) != std::string::npos)
            url += server.conf.data()[resp.specs]["server"]["error_page"][i].substr(server.conf.data()[resp.specs]["server"]["error_page"][i].find_first_not_of("\t\n\r\v\f ", 3), \
            server.conf.data()[resp.specs]["server"]["error_page"][i].length() - server.conf.data()[resp.specs]["server"]["error_page"][i].find_first_not_of("\t\n\r\v\f ", 3));
        i++;
    }
    const char *codes[8] = { "400", "403", "404", "405", "411", "413", "500", "505" };
    int errComp;
    errComp = 0;
    errComp++;
    while (j < 8)
    {
        if (err == atoi(codes[j]))
            errComp = atoi(codes[j]);
        j++;
    }
    std::string numberString = "";
    std::string prefix = url;
    if (prefix.find_first_not_of("/", 0) != std::string::npos)
        prefix = prefix.substr(prefix.find_first_not_of("/", 0), prefix.length() - prefix.find_first_not_of("/", 0));
    std::basic_ifstream<char> fs(prefix.c_str());
    std::ostringstream oss;
    oss << fs.rdbuf();
    std::string content(oss.str());
    std::ostringstream digit;
    digit << content.size();
    numberString = digit.str();

    if (err == 400)// for now for missing extension in file
        response.append("400 BAD REQUEST");
    else if (err == 403 && content.size())
        response.append("403 FORBIDDEN");
    else if (err == 404 && content.size())// for now, couldn't open file so does not exist
        response.append("404 NOT FOUND");
    else if (err == 405 && content.size())// is not GET POST or DELETE
        response.append("405 METHOD NOT ALLOWED");
    else if (err == 411 && content.size())// content lenght missing
        response.append("411 LENGTH REQUIRED");
    else if (err == 413 && content.size()) // client_max_body_size : Sets the maximum allowed size of the client request body, specified in the “Content-Length” request header field. If the size in a request exceeds the configured value, the 413 (Request Entity Too Large) error is returned to the client. Setting size to 0 disables checking of client request body size.
        response.append("413 PAYLOAD TOO LARGE");
    else if (err == 500 && content.size())// For now, couldn't delete file
        response.append("500 INTERNAL SERVER ERROR");
    else if (err == 505 && content.size())// bad hhtp protocol version
        response.append("505 HTTP VERSION NOT SUPPORTED");
    if (content.size())
        return response += " Content-Type: " + findExtension(prefix) + " Content-Length: " + numberString + "\r\n\r\n" + content;

    return set_error(err);
}

////////////////////////////////////////////////////////////////
//                           GET                              //
////////////////////////////////////////////////////////////////

std::string handle_get(Response resp, nginx server)
{
    std::string response = "HTTP/1.1 200 OK ";
    std::string numberString = "";

    std::string url = resp.absolut_path;
    size_t i = 0;
    if (url.length() && url.find("/", 0) == 0)
    {
        while (i < url.length() && url.at(i) == '/')
            i++;
        url = url.substr(i, url.length() - i);
    }
    std::basic_ifstream<char> fs(url.c_str());
    std::ostringstream oss;
    oss << fs.rdbuf();
    std::string content(oss.str());
    std::ostringstream digit;
    digit << content.size();
    numberString = digit.str();
    if (content.size())
        return response += "Content-Type: " + findExtension(resp.absolut_path) + " Content-Length: " + numberString + "\r\n\r\n" + content;
    return go_error((resp.re.error_type = 404), server, resp);
}

////////////////////////////////////////////////////////////////
//                           POST                             //
////////////////////////////////////////////////////////////////

std::string go_simple_upload(Response resp, nginx server)
{
    if (resp.re.encoding == "chunked" && resp.re.filename == "")
        return "HTTP/1.1 202 ACCEPTED";
    std::string way = "";
    std::string te;
    if (server.conf.data()[resp.specs][resp.loc]["upload_dir"].size() > 0)
	{
        struct stat s;
        if ( stat(server.conf.data()[resp.specs][resp.loc]["upload_dir"][0].c_str(), &s) == 0 )
		{
            if ( s.st_mode & S_IFDIR )
			{
                way.append(server.conf.data()[resp.specs][resp.loc]["upload_dir"][0]);
                te = way.substr(1, way.size() - 1);
                te.append("/");
            }
            else
                te = resp.absolut_path;
        }
        else
            te = resp.absolut_path;
    }
    else
        te = resp.absolut_path;
    te.append(resp.re.filename);
    if (resp.re.body == "")
        return "HTTP/1.1 204 NO CONTENT";
    std::ofstream fs;
    fs.open(te.c_str());
    fs << resp.re.body;
    fs.close();
    return "HTTP/1.1 201 CREATED";
}

std::string go_form_post(Response resp, nginx server)
{
    std::string response = "HTTP/1.1 ";
    int j = std::count(resp.re.query.c_str(), resp.re.query.c_str() + resp.re.query.size(), '&');
    if (j == 0)
        return go_error(400, server, resp);
    int k = -1;
    std::string content = resp.re.query;
    while (k < j)
	{
        std::string key = content.substr(0, content.find("="));
        std::size_t stop = content.find_first_of(" \n&\r");
        if (stop == content.npos)
		{
            std::string value = content.substr(key.size() + 1, content.size() - (key.size() + 1));
            response.append("200 OK");
            return response;
        }
        std::string value = content.substr(key.size() + 1, stop - (key.size() + 1));
        content = content.substr(stop + 1, content.size() - (stop + 1));
        k++;
    }
    return response.append("200 OK");
}

std::string go_post_check(Response resp, nginx server)
{
    if (resp.re.content_type != "multipart/form-data" && resp.re.content_type != "application/x-www-form-urlencoded")
        return go_simple_upload(resp, server);
    else
        return go_form_post(resp, server);
    return "";
}

////////////////////////////////////////////////////////////////
//                          DELETE                            //
////////////////////////////////////////////////////////////////

std::string handle_delete(Response resp, nginx server)
{
    std::string file = resp.absolut_path;
    size_t i = 0;
    if (file.length() && file.find("/", 0) == 0)
    {
        while (i < file.length() && file.at(i) == '/')
            i++;
        file = file.substr(i, file.length() - i);
    }

    std::string response = "HTTP/1.1 200 OK";

    if (remove(file.c_str()) != 0)
        return go_error((resp.re.error_type = 500), server, resp);

    return response;
}

////////////////////////////////////////////////////////////////
//                         DISPATCH                           //
////////////////////////////////////////////////////////////////

std::string get_response(Response resp, nginx server)
{

    resp.re.status_is_handled = true;
    std::string response = "HTTP/1.1 200 OK ";

    if (resp.re.error_type != 200)
        return go_error(resp.re.error_type, server, resp);

    if (resp.re.is_cgi == true)
        return handle_cgi(resp, server);

    if (server.conf.data()[resp.specs][resp.loc]["return"].size() > 0)
        return go_redirect(resp, server);

    std::string respn = go_directory(resp, server);

    if (respn != "")
        return respn;

    if (resp.re.type == "POST")
        return go_post_check(resp, server);

    if (resp.re.type == "DELETE")
        return handle_delete(resp, server);

    if (resp.re.type == "GET")
        return handle_get(resp, server);

    return go_error((resp.re.error_type = 405), server, resp);

}