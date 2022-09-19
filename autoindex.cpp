#include <iostream>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysmacros.h>


// The ngx_http_autoindex_module module processes requests ending with the slash character (‘/’)
// and produces a directory listing.
// Usually a request is passed to the ngx_http_autoindex_module module when the ngx_http_index_module
//  module cannot find an index file.


// DIRECTIVES:
// autoindex:
		// Syntax:	autoindex on | off;
		// Default:
		// autoindex off;
		// Context:	http, server, location

// autoindex_exact_size:
	// For the HTML format, specifies whether exact file sizes should be output
	// in the directory listing, or rather rounded to kilobytes, megabytes, and gigabytes.
		// Syntax:	autoindex_exact_size on | off;
		// Default:
		// autoindex_exact_size on;
		// Context:	http, server, location

// autoindex_format:
	// Sets the format of a directory listing.
	// When the JSONP format is used, the name of a callback function is set with the callback request argument.
	// If the argument is missing or has an empty value, then the JSON format is used.
		// Syntax:	autoindex_format html | xml | json | jsonp;
		// Default:
		// autoindex_format html;
		// Context:	http, server, location

// autoindex_localtime:
	// For the HTML format, specifies whether times
	// in the directory listing should be output in the local time zone or UTC.
		// Syntax:	autoindex_localtime on | off;
		// Default:
		// autoindex_localtime off;
		// Context:	http, server, location

typedef enum e_file_type {FILE_TYPE, DIRECTORY, SYMLINK, UNKNOWN} file_type;

class File
{
public:
	bool				valid;
	std::string 		name;
	std::string 		path;
	std::string 		URI;
	std::string 		time_stamp_str;
	long 				time_stamp_raw;
	file_type			type;
	unsigned long long	size;
	unsigned long long	IO_read_block;
	File(std::string target, std::string folder) : name(target), path(folder)
	{
		std::stringstream target_uri;
		target_uri << folder << "/" << name;
		URI = target_uri.str();
	};
};

class Autoindex
{
private:
	 const char *_html_start = "<!doctype html><html><header><style></style></header><body>";
	 const char *_html_end = "</body></html>";
	 // const char *_html_title_start = "<h3>";
	 // const char *_html_title_end = "</h3>";
	 const char *_html_a_start = "<div><a type=\"text/html\" href=\"";
	 const char *_html_a_end = "</a></div>";

	std::string _create_link(File file)
	{
		std::stringstream output;
		output << _html_a_start << "/" << file.URI << "\">" << file.name << _html_a_end;
		return output.str();
	};
public:
	std::vector<File> files;

	Autoindex(std::vector<File> f) : files(f)
	{

	};
	std::string out()
	{
		std::stringstream output;
		output << _html_start;
		for (std::vector<File>::iterator it = files.begin(); it != files.end(); it++) {
			output << _create_link(*it);
		}
		output << _html_end;
		return output.str();
	};
};

void printInfos(const File &info)
{
	std::cout << info.URI << " is valid: "<< info.valid << "\t";
	std::cout << "last modification: "<< info.time_stamp_str << "\t";
	std::cout << " - "<< info.time_stamp_raw << "\t";
	std::cout << "type: "<< (info.type == DIRECTORY ? "DIR \t" : "File\t");
	std::cout << "IO_size: " << info.IO_read_block << "\t"<< "size: "<< info.size << "\n";
};

File get_file_infos(std::string target, int folder, std::string path)
{
	struct stat infos;
	File target_infos(target, path);

	if(fstatat(folder, target.c_str(), &infos, 0) == 0)
	{
		target_infos.valid = true;
		target_infos.time_stamp_str = ctime(&infos.st_mtime);
		target_infos.time_stamp_raw = infos.st_mtime;
		target_infos.size = infos.st_size;
		target_infos.IO_read_block = infos.st_blksize;
		target_infos.IO_read_block = infos.st_blksize;
		if(infos.st_mode & S_IFDIR) //it's a directory
			target_infos.type = DIRECTORY;
		else if(infos.st_mode & S_IFREG) //it's a file
			target_infos.type = FILE_TYPE;
		else if(infos.st_mode & S_IFLNK) //it's a symlink
			target_infos.type = SYMLINK;
		else //something else
			target_infos.type = UNKNOWN;
	}
	else
		target_infos.valid = false;
	return target_infos;
};

std::vector<File> ls(char const *target)
{
	DIR *folder;
	struct dirent *entry;
	std::vector<File> filelist;
	if (!(folder = opendir(target)))
		return filelist;
	while ((entry = readdir(folder)))
		filelist.push_back(get_file_infos(entry->d_name, dirfd(folder), target));
	closedir(folder);
	return filelist;
};


int main(int argc, char const *argv[]) {
	(void)argc;
	std::vector<File> filelist = ls(argv[1]);
	// for (std::vector<File>::iterator it = filelist.begin(); it != filelist.end(); it++) {
		// printInfos(*it);
	// }
	std::cout << Autoindex(filelist).out() << std::endl;
	return 0;
}
