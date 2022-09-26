#pragma once
#include "Request.hpp"

class Client;

class Location {
	public:
	Location();
	Location(const Location &cpy);
	~Location();
	Location &operator=(const Location &cpy);

	bool							get_method;
	bool							post_method;
	bool							delete_method;
	std::string						redirect;
	std::string						root;
	bool							autoindex;
	std::string						default_file;
	std::string						CGI;
	std::pair<bool, std::string>	upload;
};

class Server_block {
	public:
	Server_block();
	Server_block(const Server_block &cpy);
	~Server_block();
	Server_block &operator=(const Server_block &cpy);

	int										port;			// Mandatory
	std::string								address;		// Mandatory
	std::string								server_names;	// Optional
	bool									main; 			// Le premier serveur pour un host:port sera le serveur par défaut pour cet host:port (ce qui signifie qu’il répondra à toutes les requêtes qui n’appartiennent pas à un autre serveur).
	std::map<int, std::string>				error_pages;	// Optional
	std::string								root;
	size_t									body_size;		// Optional ? (setup default value)
	std::vector<Location>					locations;		// Optional
};


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
	// typedef enum e_error {NOT_FOUND = -1, OK, FORBIDDEN_R, FORBIDDEN_W, INVALID_NAME} error;
	file_type			type;
	unsigned long long	size;
	unsigned long long	IO_read_block;
	std::string			content;
	std::string			ext;
	std::string			mime_type;
	char				permissions:3;
	bool				not_found;

	File(void);
	File(std::string target, std::string folder);
	File(const File &src);
	File &operator=(const File &src);

	~File();
	void set_permissions();
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
