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
	~File();
};

File get_file_infos(std::string target, int folder, std::string path);
void printFileInfos(const File &info);
