#include <autoindex.hpp>

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

std::string Autoindex::_create_link(File file)
{
	std::stringstream output;
	output
	<< _tr
		<< _td
			<< (file.type == DIRECTORY ? _html_folder_icon : _html_file_icon)
		<< _ntd
		<< _td
			<< _html_a_start << "/"
			<< file.URI << "\">"
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
	return output.str();
};

Autoindex::Autoindex(std::vector<File> f) : files(f) {};

Autoindex::Autoindex(char const *relative_path_only)
{
	ls(relative_path_only);
};

Autoindex::~Autoindex(void) {};

void Autoindex::ls(char const *target)
{
	DIR *folder;
	struct dirent *entry;
	std::vector<File> filelist;
	if (!(folder = opendir(target)))
		return ;
	while ((entry = readdir(folder)))
		filelist.push_back(get_file_infos(entry->d_name, dirfd(folder), target));
	closedir(folder);
	files = filelist;
};

std::pair<std::string, size_t> Autoindex::out(void)
{
	std::stringstream output;
	std::string html;
	output << _html_start << "<table style=\"width:100%; text-align:left; vertical-align: middle;\">";
	output << _th << "" << _nth << _th << "name" << _nth << _th << "size (bytes)" << _nth << _th << "Last Modified" << _nth;
	for (std::vector<File>::iterator it = files.begin(); it != files.end(); it++) {
		output << _create_link(*it);
	}
	output << "</table>";
	output << _html_end;
	html = output.str();
	return std::make_pair(html, html.size());
};

void printFileInfos(const File &info)
{
	std::cout << info.URI << " is valid: "<< info.valid << "\t";
	std::cout << "last modification: "<< info.time_stamp_str << "\t";
	std::cout << " - "<< info.time_stamp_raw << "\t";
	std::cout << "type: "<< (info.type == DIRECTORY ? "DIR \t" : "File\t");
	std::cout << "IO_size: " << info.IO_read_block << "\t"<< "size: "<< info.size << "\n";
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

// int main(int argc, char const *argv[]) {
// 	(void)argc;
// 	std::cout << Autoindex(argv[1]).out().first << std::endl;
// 	return 0;
// }
