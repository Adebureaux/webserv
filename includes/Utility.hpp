#pragma once
#include "Request.hpp"

class Client;

class Message
{
	public:
	Client			*client;
	t_state			state;
	std::string		raw_data;
	Message			*ptr; // response || request
	Request			info;

	Message(Client *c);
	virtual ~Message();
	const char *data(void) const;// should return complete response buffer; wether it's an error or correct page
	size_t size(void) const; // should return response buffer size
};


class File
{
	public:
	std::string 		name;
	std::string 		path;
	std::string 		uri; // = path + / + name
	std::string 		time_stamp_str;
	long 				time_stamp_raw;
	bool				valid;
	file_type			type;
	unsigned long long	size;
	unsigned long long	IO_read_block;
	std::string			content;
	std::string			ext;
	std::string			mime_type;
	File(std::string target, std::string folder);
	File(const File &src);
	File &operator=(const File &src);

	~File();

	void set_content(void);
	void set_mime_type(void);

	struct entry
	{
		const std::string& ext;
		const std::string& mime_type;
	};
	static File::entry types[MIME_TYPE_NUMBER];
	
	//void get_mime_type(void);
	// TODO ==> Create 2 functions :
	// find_type() --> give the file extention with the mime type (example text/html for .html file)
};

File get_file_infos(std::string target, std::string path, int folder_fd);
void printFileInfos(const File &info);
std::vector<File> ls(char const *root);
