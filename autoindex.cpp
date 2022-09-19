#include <iostream>
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

class Infos
{
public:
	bool				valid;
	std::string 		name;
	std::string 		time_stamp_str;
	long 				time_stamp_raw;
	file_type			type;
	unsigned long long	size;
	unsigned long long	IO_read_block;
	Infos(std::string target) { name = target;};
};

void printInfos(const Infos &info)
{
	std::cout << info.name << " is valid: "<< info.valid << "\t";
	std::cout << "last modification: "<< info.time_stamp_str << "\t";
	std::cout << " - "<< info.time_stamp_raw << "\t";
	std::cout << "type: "<< (info.type == DIRECTORY ? "DIR \t" : "File\t");
	std::cout << "IO_size: " << info.IO_read_block << "\t"<< "size: "<< info.size << "\n";
};

Infos get_file_infos(std::string target, int folder)
{
	struct stat infos;
	Infos target_infos(target);

	if(fstatat(folder, target.c_str(), &infos, 0) == 0)
	{
		target_infos.valid = true;
		target_infos.time_stamp_str = ctime(&infos.st_mtime);
		target_infos.time_stamp_raw = infos.st_mtime;
		target_infos.size = infos.st_size;
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

std::vector<Infos> ls(char const *target)
{
	DIR *folder;
	struct dirent *entry;
	std::vector<Infos> filelist;
	if (!(folder = opendir(target)))
		return filelist;
	while ((entry = readdir(folder)))
		filelist.push_back(get_file_infos(entry->d_name, dirfd(folder)));
	closedir(folder);
	return filelist;
};

int main(int argc, char const *argv[]) {
	(void)argc;
	std::vector<Infos> filelist = ls(argv[1]);
	for (std::vector<Infos>::iterator it = filelist.begin(); it != filelist.end(); it++) {
		printInfos(*it);
	}
	return 0;
}
