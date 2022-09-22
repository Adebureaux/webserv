#include "Utility.hpp"
// #include "../includes/Utility.hpp"

Message::Message(Client *c) :
	client(c),
	state(INCOMPLETE),
	raw_data(""), // ! PLACEHOLDER
	ptr(NULL),
	info("DEFAULT")
{};

Message::~Message() {};

const char *Message::data(void) const
{
	return raw_data.c_str();// PLACEHOLDER !!
}; // should return complete response buffer; wether it's an error or correct page

size_t Message::size(void) const
{
	return raw_data.size();// PLACEHOLDER !!
}; // should return response buffer size

File::File(std::string name, std::string path) : name(name), path(path)
{
	struct stat infos;
	std::stringstream target_uri;

	valid = false;
	if (path.empty())
		target_uri << name;
	else
		target_uri << path << "/" << name;
	uri = target_uri.str();
	if(stat(uri.c_str(), &infos) == 0)
	{
		valid = true;
		time_stamp_str = ctime(&infos.st_mtime);
		time_stamp_raw = infos.st_mtime;
		size = infos.st_size;
		IO_read_block = infos.st_blksize;
		if(infos.st_mode & S_IFDIR) //it's a directory
			type = DIRECTORY;
		else if(infos.st_mode & S_IFREG) //it's a file
			type = FILE_TYPE;
		else if(infos.st_mode & S_IFLNK) //it's a symlink
			type = SYMLINK;
		else //something else
			type = UNKNOWN;
	}
};

File::File(const File &src)
{
	name = src.name;
	path = src.path;
	uri = src.uri;
	type = src.type;
	IO_read_block = src.IO_read_block;
	size = src.size;
	content = src.content;
	valid = src.valid;
	time_stamp_raw = src.time_stamp_raw;
	time_stamp_str = src.time_stamp_str;
	mime_type = src.mime_type;
};

File &File::operator=(const File &src)
{
	name = src.name;
	path = src.path;
	uri = src.uri;
	type = src.type;
	IO_read_block = src.IO_read_block;
	size = src.size;
	content = src.content;
	valid = src.valid;
	time_stamp_raw = src.time_stamp_raw;
	time_stamp_str = src.time_stamp_str;
	mime_type = src.mime_type;
	return *this;
};

File::~File() {};

void File::get_content()
{
	std::ifstream file(uri.c_str());
	std::stringstream ssbuffer;
	if (file.rdbuf()->is_open())
	{
		ssbuffer << file.rdbuf();
		content = ssbuffer.str();
		file.close();
	}
	else
		valid = false;
};

File get_file_infos(std::string target, std::string path)
{
	File target_infos(target, path);

	return target_infos;
};

std::vector<File> ls(char const *root)
{
	DIR *folder;
	struct dirent *file;
	std::vector<File> filelist;
	if (!(folder = opendir(root)))
		return filelist;
	while ((file = readdir(folder)))
	{
		File newfile = File(file->d_name, root);
		filelist.push_back(newfile);
	}
	closedir(folder);
	return filelist;
};
