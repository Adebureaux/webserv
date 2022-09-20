#include "Utility.hpp"

// PLACEHOLDER: should instead

Message::Message(Client *c) :
	client(c),
	state(INCOMPLETE),
	raw_data(""), // ! PLACEHOLDER
	// header(""),
	// body(""),
	ptr(NULL),
	info("DEFAULT")
{};

Message::~Message(){};

const char *Message::data(void) const
{
	return raw_data.c_str();// PLACEHOLDER !!
}; // should return complete response buffer; wether it's an error or correct page

size_t Message::size(void) const
{
	return raw_data.size();// PLACEHOLDER !!
}; // should return response buffer size

File::File(std::string target, std::string folder) : name(target), path(folder)
{
	std::stringstream target_uri;
	target_uri << folder << "/" << name;
	URI = target_uri.str();
};

File::~File(void){};

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
