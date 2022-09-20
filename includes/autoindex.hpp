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
	File(std::string target, std::string folder);
	~File(void);
};

File get_file_infos(std::string target, int folder, std::string path);
void printFileInfos(const File &info);


class Autoindex
{
private:
	static const char *_html_start;
	static const char *_html_end;
	static const char *_html_a_start;
	static const char *_html_a_end;
	static const char *_html_file_icon;
	static const char *_html_folder_icon;
	static const char *_td;
	static const char *_ntd;
	static const char *_th;
	static const char *_nth;
	static const char *_tr;
	static const char *_ntr;

	std::string _create_link(File file);
public:
	std::vector<File> files;

	Autoindex(std::vector<File> f);
	Autoindex(char const *relative_path_only);
	~Autoindex(void);

	void ls(char const *target);
	std::pair<std::string, size_t> to_html(void);
};

std::vector<File> ls(char const *target);
