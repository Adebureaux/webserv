#pragma once
#include "Request.hpp"

class Client;


class Location {
	public:
	Location();
	Location(const Location &cpy);
	~Location();
	Location &operator=(const Location &cpy);

	bool							get_method;			// Optional
	bool							post_method;		// Optional
	bool							delete_method;		// Optional
	std::string						redirect;			// Optional
	std::string						root;				// Optional -> Takes the URI by default
	bool							autoindex;			// Optional -> False by default
	std::string						default_file;		// Optional -> index.html by default
	std::string						CGI;				// Optional
	std::pair<bool, std::string>	upload;				// Optional
	std::string 					uri;				// Mandatory -> element map.first in location_map
};

typedef std::map<std::string, Location> location_map;

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
	size_t									body_size;		// Optional ? (setup default value)
	location_map							locations;		// Mandatory
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
	~Message();
};


class File
{
	public:
	std::string 		name;
	// std::string 		location_name; // the file/folder name which has been requested
										// needed for autoindex links creations and to build responses
	std::string 		path; // the complete file/folder path which has been requested
										// needed for autoindex links creations and to build responses
	// std::string 		absolute_path; // MUST contain the complete absolute path : "/var/www/index.html"
	//										this is this variable which will serve to open the file
	std::string 		uri; // = path + / + name
	bool				valid;
	// typedef enum e_error {NOT_FOUND = -1, OK, FORBIDDEN_R, FORBIDDEN_W, INVALID_NAME} error;

	std::string 		time_stamp_str;
	long 				time_stamp_raw;
	file_type			type;
	unsigned long long	size;
	unsigned long long	IO_read_block;
	std::string			content;
	std::string			ext;
	std::string			mime_type;
	char				permissions:3;
	bool				not_found;

	File(void);
	File(std::string filename);
	File(std::string target, std::string folder);
	File(const File &src);
	File &operator=(const File &src);
	~File();
	void set_permissions(void);
	void set_infos(void);
	void set_content(void);
	void set_mime_type(void);
	struct entry
	{
		const std::string& ext;
		const std::string& mime_type;
	};
	static const entry types[MIME_TYPE_NUMBER];
};

File get_file_infos(std::string target, std::string path, int folder_fd);
void printFileInfos(const File &info);
std::vector<File> ls(char const *root);
std::string find_path(const std::string& uri);
std::string find_basename(const std::string& uri);
