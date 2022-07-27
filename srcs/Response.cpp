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

////////////////////////////////////////////////////////////////
//                          ERROR                             //
////////////////////////////////////////////////////////////////

std::string go_error(int err, Bundle_for_response bfr)
{
	(void)bfr;

	std::string error = "go_error, int error = " + err;

	return (error);
}

////////////////////////////////////////////////////////////////
//                           GET                              //
////////////////////////////////////////////////////////////////

std::string handle_get(Bundle_for_response bfr)
{
	bfr.absolut_path = "//formulaire.html";	////////// a effacer //////// 
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

////////////////////////////////////////////////////////////////
//                           POST                             //
////////////////////////////////////////////////////////////////

std::string go_simple_upload(Bundle_for_response bfr, serverConf conf){
    if (bfr.re.encoding == "chunked" && bfr.re.filename == "")
        return "HTTP/1.1 202 ACCEPTED";
    std::string way = "";
    std::string te;
    if (conf.http.data()[bfr.specs][bfr.loc]["upload_dir"].size() > 0){
        struct stat s;
        if ( stat(conf.http.data()[bfr.specs][bfr.loc]["upload_dir"][0].c_str(), &s) == 0 ){
            if ( s.st_mode & S_IFDIR ){
                way.append(conf.http.data()[bfr.specs][bfr.loc]["upload_dir"][0]);
                te = way.substr(1, way.size() - 1);
                te.append("/");
            }
            else
                te = bfr.absolut_path;
        }
        else
            te = bfr.absolut_path;
    }
    else
        te = bfr.absolut_path;
    te.append(bfr.re.filename);
    if (bfr.re.body == "")
        return "HTTP/1.1 204 NO CONTENT";
    std::ofstream fs;
    fs.open(te.c_str());
    fs << bfr.re.body;
    fs.close();
    return "HTTP/1.1 201 CREATED";
}

std::string go_form_post(Bundle_for_response bfr, serverConf conf){
    std::string response = "HTTP/1.1 ";
    int j = std::count(bfr.re.query.c_str(), bfr.re.query.c_str() + bfr.re.query.size(), '&');
    if (j == 0)
        return go_error(400, conf, bfr);
    int k = -1;
    std::string content = bfr.re.query;
    while (k < j){
        std::string key = content.substr(0, content.find("="));
        std::size_t stop = content.find_first_of(" \n&\r");
        if (stop == content.npos){
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

std::string go_post_check(Bundle_for_response bfr, serverConf conf){
    if (bfr.re.content_type != "multipart/form-data" && bfr.re.content_type != "application/x-www-form-urlencoded")
        return go_simple_upload(bfr, conf);
    else
        return go_form_post(bfr, conf);
    return "";
}

////////////////////////////////////////////////////////////////
//                         DISPATCH                           //
////////////////////////////////////////////////////////////////

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

	if (resp != "")
        return resp;
    if (bfr.re.type == "POST")
        return go_post_check(bfr, conf);
    if (bfr.re.type == "GET")
        return handle_get(bfr);

    return go_error((bfr.re.error_type = 405), bfr);
}