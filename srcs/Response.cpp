#include "../includes/Response.hpp"

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

std::string go_error(int err, Bundle_for_response bfr)
{
	(void)bfr;

	std::string error = "go_error, int error = " + err;

	return (error);
}

std::string handle_get(Bundle_for_response bfr)
{
	bfr.absolut_path = "//index.html";	////////// a effacer ////////
	// absolute path parsed in main still gotta make it so just use 
	// index.html for testing.

    std::string response = "HTTP/1.1 200 OK\n";
    std::string numberString = "";
    std::string url = bfr.absolut_path;

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
	fs.close();
    std::string content(oss.str());
    std::ostringstream digit;
    digit << content.size();
    numberString = digit.str();

    if (content.size())
        return response += "Content-Type: " + findExtension(bfr.absolut_path) + "Content-Length: " + numberString + "\r\n\r\n" + content;

    return go_error((bfr.re.error_type = 404), bfr);
}

std::string get_response(Bundle_for_response bfr)	// ,serverConf conf ??
{
    bfr.re.status_is_handled = true;


	//std::string response = "HTTP/1.1 200 OK ";
    // if (bfr.re.error_type != 200)
    //     return go_error(bfr.re.error_type, conf, bfr);
    // if (bfr.re.is_cgi == true)
    //     return handle_cgi(bfr, conf);
    // if (conf.http.data()[bfr.specs][bfr.loc]["return"].size() > 0)
    //     return go_redirect(bfr, conf);
    // std::string resp = go_directory(bfr, conf);

    // if (resp != "")
    //     return resp;


    if (bfr.re.type == "GET")
        return handle_get(bfr);

    return go_error((bfr.re.error_type = 405), bfr);
}