#include "Autoindex.hpp"

const char* Autoindex::_html_start = "<!doctype html><html><header><style>body {padding: 5vh;} a {vertical-align:middle;} img {position:relative;vertical-align: middle;height: 15px; margin-right:20px;}</style></header><body>";
const char* Autoindex::_html_end = "</body></html>";
const char* Autoindex::_html_a_start = "<a type=\"text/html\" href=\"";
const char* Autoindex::_html_a_end = "</a>";
const char* Autoindex::_html_file_icon = "<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAABmJLR0QA/wD/AP+gvaeTAAABIUlEQVRoge2ZSwrCMBBAn4Kix1UQd8UeQRfu9DJ6C0W8hmDRhS2E4qdxJhnQeTDQlCYzj5Y0TcFxnC6MgRI4AlfgFhkXYAn0cxcOj+L3EcW+iw0GEqVC4aYSxyD5DBhE9jeXCJ/54Rf9ze9EmFTaf/OknVxCU6AHrFvntiSW0BQAAwltAcgskUIAMkqkEoBMElKBT9Nwn8Szk1QgfBHOMZCQCixaY8TESlJ4g1RAshi8SApvkAoAjIACOBC/HH9Lr0Py9vydg845TT4wNHEBa1zAGhewxgWscQFrXMAaF7DGBaz5C4EqOP5mez2WMEf18qqaLgLn4HhCWokhMA3aJ41BJfs60ig0BDR/8sXEjsd2jAqSfZ2YuNY5Cs3iHeeXuQO5ISyjPwVqawAAAABJRU5ErkJggg==\">";
const char* Autoindex::_html_folder_icon = "<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAABmJLR0QA/wD/AP+gvaeTAAABWUlEQVRoge2YXUrDUBBGjz8NdgFZhNAX11GXIb77+xhaV6ZY0DVoKYI7EPoiKfqQBibpbZJr7uQ+OAcGUnJvOF8ySWHAMAzDMHYZA3NgCeTAT0OtgVfgEjiKIVtnDCxolt5XT0A6vHKVOX+TL2sFTAa3FiyFzB0walmfUoSWrfYFnCs6NiJFEo99Uwrxcm8O3Aa364BsB18mFC3UpwVl5RQdMaN4N9UDAJwC3wFDlLUATrQDhH4C9co0A7jegRvPa9QZAffimm9dNvkGSIEHdr9CU0/ZfSRUb0orfR/zJ3AWSN7l5LXYtx7R+SdWDbAGXoAL4DCwuMupwnHLxgMVnYBo3bHBsACxsQCxsQCxsQCxsQCxsQCxcQXYiGOfsYoW0mFTP+kK8CGOr4gbIgGuxe9Vl00z9KYKg0wl+gx3NeuZjnMhtgsz4J328bpm5VuHzEfeMAzD+D/8AvAyR2dQGWtEAAAAAElFTkSuQmCC\">";
const char* Autoindex::_td = "<td>";
const char* Autoindex::_ntd = "</td>";
const char* Autoindex::_th = "<th>";
const char* Autoindex::_nth = "</th>";
const char* Autoindex::_tr = "<tr>";
const char* Autoindex::_ntr = "</tr>";

std::string Autoindex::_create_href(File file, std::string pseudo_root)
{
	// (void)file;
	// (void)pseudo_root;
	std::stringstream output;
	std::string temp;
	std::cout << pseudo_root << std::endl;
	// if (pseudo_root.empty())
		output << "/";
	if (file.name == ".")
		output << pseudo_root;
	else if (file.name == "..")
	{
		std::string::iterator it = pseudo_root.end();
		it--;
		if (pseudo_root.size() > 1 && *it == '/')
		{
			it--;
			while (it != pseudo_root.begin() && *it != '/')
				it--;
			std::string new_root;
			new_root.insert(new_root.begin(), pseudo_root.begin(), it);
			output << new_root;
			if (new_root.size())
				output << "/";
		}
	}
	else if (pseudo_root != "/")
		output << file.name;
	else
		output << pseudo_root << file.name;
	if (file.type == DIRECTORY && file.name != "." && file.name != "..")
		output << "/";
	std::cout << output.str() << std::endl;

	return output.str();
};

std::string Autoindex::_create_link(File file, const std::string &pseudo_root)
{
	std::stringstream output;
	std::cout << C_G_BLUE << "AUTOINDEX URI = |"
	<< file.name << "| path = |" << file.path
	<< "| uri = |" << file.uri << "| pseudo_root |"  << pseudo_root << "|" << C_RES << std::endl;

	output
	<< _tr
		<< _td
			<< (file.type == DIRECTORY ? _html_folder_icon : _html_file_icon)
		<< _ntd
		<< _td
			<< _html_a_start
			<< _create_href(file, pseudo_root) << "\">"
			<< file.name
			<< _html_a_end
		<< _ntd
		<< _td;
			if (file.type == FILE_TYPE)
				output << file.size;
			else
				output << "";
		output << _ntd
		<< _td
			<< file.time_stamp_str
		<< _ntd
	<< _ntr;
	std::cout << output.str() << std::endl;
	return output.str();
};

Autoindex::Autoindex(std::vector<File> f) : files(f) {};

Autoindex::Autoindex(char const *relative_path_only)
{
	ls(relative_path_only);
};

Autoindex::~Autoindex(void) {};

void Autoindex::ls(char const *root)
{
	DIR *folder;
	struct dirent *file;
	std::vector<File> filelist;
	if (!(folder = opendir(root)))
		return ;
	while ((file = readdir(folder)))
	{
		File newfile = File(file->d_name, root);
		filelist.push_back(newfile);
	}
	closedir(folder);
	files = filelist;
};

bool FileCompare(const File &a, const File &b)
{
	if (a.type == DIRECTORY && b.type == DIRECTORY && a.name < b.name)
		return true;
	else if (a.type == DIRECTORY && b.type == DIRECTORY && a.name > b.name)
		return false;
	else if (a.type == DIRECTORY && b.type != DIRECTORY)
		return true;
	else if (b.type == DIRECTORY && a.type != DIRECTORY)
		return false;
	else if (b.type != DIRECTORY && a.type != DIRECTORY && a.name < b.name)
		return true;
	return false;
};

std::pair<std::string, size_t> Autoindex::to_html(const std::string &pseudo_root)
{
	std::stringstream output;
	std::string html;
	std::vector<File>::iterator it = files.begin();
	std::sort(files.begin(), files.end(), &FileCompare);
	// it->
	output << _html_start << "<table style=\"width:100%; text-align:left; vertical-align: middle;\">";
	output << _th << "" << _nth << _th << "name" << _nth << _th << "size (bytes)" << _nth << _th << "Last Modified" << _nth;
	for (; it != files.end(); it++) {
		printFileInfos(*it);
		output << _create_link(*it, pseudo_root);
	}
	output << "</table>";
	output << _html_end;
	html = output.str();
	return std::make_pair(html, html.size());
};

// int main(int argc, char const *argv[]) {
// 	(void)argc;
// 	std::cout << Autoindex(argv[1]).to_html().first << std::endl;
// 	return 0;
// }
