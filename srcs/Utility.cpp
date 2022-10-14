#include "Utility.hpp"

Location::Location()
: get_method(false), post_method(false), delete_method(false), redirect(), root(), autoindex(false), default_file(""), CGI(), upload(false, ""), uri()
{}

Location::Location(const Location &cpy)
{
	*this = cpy;
}

Location::~Location(){}

Location &Location::operator=(const Location &cpy)
{
	get_method = cpy.get_method;
	post_method = cpy.post_method;
	delete_method = cpy.delete_method;
	redirect = cpy.redirect;
	root = cpy.root;
	autoindex = cpy.autoindex;
	default_file = cpy.default_file;
	CGI = cpy.CGI;
	upload = cpy.upload;
	uri = cpy.uri;
	return (*this);
}

Server_block::Server_block()
: port(-1), address(), server_names(), main(false), error_pages(), body_size(BUFFER_SIZE), locations()
{}

Server_block::Server_block(const Server_block &cpy)
{
	*this = cpy;
}

Server_block::~Server_block()
{}

Server_block &Server_block::operator=(const Server_block &cpy)
{
	port = cpy.port;
	address = cpy.address;
	server_names = cpy.server_names;
	main = cpy.main;
	error_pages = cpy.error_pages;
	body_size = cpy.body_size;
	locations = cpy.locations;
	return (*this);
}

Message::Message(Client *c) :
	client(c),
	state(INCOMPLETE),
	raw_data(""), // ! PLACEHOLDER
	ptr(NULL),
	header_parsed(false),
	info("DEFAULT"),
	header_end(std::string::npos),
	response_override(0),
	continue_100(UNDEFINED),
	indicated_content_size(0),
	current_content_size(0),
	multipart(false),
	boundary(""),
	boundary_end(""),
	post_options_set(false),
	isCGI(false),
	isUpload(false)
{};

void Message::reset(void)
{
	raw_data.clear();
	state = INCOMPLETE;
	header_parsed = false,
	header_end = 0;
	response_override = 0;
	continue_100 = UNDEFINED;
	indicated_content_size = 0;
	current_content_size = 0;
	multipart = false;
	boundary.clear();
	boundary_end.clear();
	post_options_set = false;
	isCGI = false;
	isUpload = false;
};

Message::~Message() {};

File::File(std::string name, std::string path) : name(name), path(path), valid(false), type(UNKNOWN), permissions(0), not_found(false), redirect(false)
{
	if (!path.empty() && *(path.end() - 1) != '/')
		uri = path + '/' + name;
	else
		uri = path + name;
	if (access(uri.c_str(), F_OK))
		not_found = true;
	else
		set_infos();
	// std::cout << C_G_RED << "IN FILE = |" << name << "| path = |" << path << "| uri = |" << uri << "|" << C_RES << std::endl;
};

void File::set_infos()
{
	struct stat infos;

	set_permissions();
	if (!stat(uri.c_str(), &infos))
	{
		valid = true;
		time_stamp_str = std::ctime(&infos.st_mtime);
		time_stamp_raw = infos.st_mtime;
		size = infos.st_size;
		IO_read_block = infos.st_blksize;
		if (S_ISDIR(infos.st_mode))
		{
			if (*uri.rbegin() != '/')
			{
				redirect = true;
				valid = false;
			}
			type = DIRECTORY;
		}
		else if (S_ISREG(infos.st_mode) || S_ISLNK(infos.st_mode))
			type = FILE_TYPE;
	}
}

File::File(std::string filename)
{
	*this = File(find_basename(filename), find_path(filename));
}

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
	permissions = src.permissions;
	not_found = src.not_found;
	redirect = src.redirect;
};
File::File(void)
{
	name = "";
	path = "";
	uri = "";
	type = UNKNOWN;
	IO_read_block = 0;
	size = 0;
	content = "";
	valid = true;
	time_stamp_raw = 0;
	time_stamp_str = "";
	permissions = 0;
	not_found = false;
	redirect = false;
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
	permissions = src.permissions;
	not_found = src.not_found;
	redirect = src.redirect;
	return *this;
};

File::~File() {};


void File::set_permissions(void)
{
	if (!access(uri.c_str(), R_OK))
		permissions |= R;
	if (!access(uri.c_str(), W_OK))
		permissions |= W;
	if (!access(uri.c_str(), X_OK))
		permissions |= X;
};

void File::set_content(void)
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

void File::set_mime_type(void)
{
	size_t pos = name.rfind('.');

	if (pos != std::string::npos)
	{
		pos++;
		ext = name.substr(pos);
		for (int i = 0; i < MIME_TYPE_NUMBER; i++)
		{
			if (ext == types[i].ext)
			{
				mime_type = types[i].mime_type;
				return;
			}
		}
	}
	mime_type = "text/plain";
}

File get_file_infos(std::string target, std::string path)
{
	File target_infos(target, path);

	return target_infos;
};

void printFileInfos(const File &info)
{
	std::cout << C_G_CYAN<<"URI:" << info.uri << " is valid: "<< info.valid << "\t" << "name:" << info.name << "\t";
	std::cout << "last modification: "<< info.time_stamp_str << "\t";
	std::cout << " - "<< info.time_stamp_raw << "\t";
	std::cout << "type: "<< (info.type == DIRECTORY ? "DIR \t" : "File\t");
	std::cout << "IO_size: " << info.IO_read_block << "\t"<< "size: "<< info.size << "\n" << C_RES;
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

std::string find_basename(const std::string& uri)
{
	//std::cout << C_G_RED << uri << std::endl;
	return (uri.substr(uri.find_last_of("/") + 1));
}

std::string find_path(const std::string& uri)
{
	std::size_t pos = uri.find_last_of("/");
	if (pos == std::string::npos)
		return ("");
	return (uri.substr(0, pos + 1));
}

const std::string find_mime_type(const std::string& filename)
{
	size_t pos = filename.rfind('.');
	std::string ext;

	if (pos != std::string::npos)
	{
		pos++;
		ext = filename.substr(pos);
		std::cout << ext << std::endl;
		for (int i = 0; i < MIME_TYPE_NUMBER; i++)
		{
			if (ext == File::types[i].ext)
				return (File::types[i].mime_type);
		}
	}
	return ("text/plain");
}

const File::entry File::types[MIME_TYPE_NUMBER] = {
	{"php", "text/php"},
	{"conf", "webserv/conf"},
	{"ico", "image/x-icon"},
	{"*3gpp", "audio/3gpp"},
	{"*jpm", "video/jpm"},
	{"*mp3", "audio/mp3"},
	{"*rtf", "text/rtf"},
	{"*wav", "audio/wave"},
	{"*xml", "text/xml"},
	{"3g2", "video/3gpp2"},
	{"3gp", "video/3gpp"},
	{"3gpp", "video/3gpp"},
	{"ac", "application/pkix-attr-cert"},
	{"adp", "audio/adpcm"},
	{"ai", "application/postscript"},
	{"apng", "image/apng"},
	{"appcache", "text/cache-manifest"},
	{"asc", "application/pgp-signature"},
	{"atom", "application/atom+xml"},
	{"atomcat", "application/atomcat+xml"},
	{"atomsvc", "application/atomsvc+xml"},
	{"au", "audio/basic"},
	{"aw", "application/applixware"},
	{"bdoc", "application/bdoc"},
	{"bin", "application/octet-stream"},
	{"bmp", "image/bmp"},
	{"bpk", "application/octet-stream"},
	{"buffer", "application/octet-stream"},
	{"ccxml", "application/ccxml+xml"},
	{"cdmia", "application/cdmi-capability"},
	{"cdmic", "application/cdmi-container"},
	{"cdmid", "application/cdmi-domain"},
	{"cdmio", "application/cdmi-object"},
	{"cdmiq", "application/cdmi-queue"},
	{"cer", "application/pkix-cert"},
	{"cgm", "image/cgm"},
	{"class", "application/java-vm"},
	{"coffee", "text/coffeescript"},
	{"conf", "text/plain"},
	{"cpt", "application/mac-compactpro"},
	{"crl", "application/pkix-crl"},
	{"css", "text/css"},
	{"csv", "text/csv"},
	{"cu", "application/cu-seeme"},
	{"davmount", "application/davmount+xml"},
	{"dbk", "application/docbook+xml"},
	{"deb", "application/octet-stream"},
	{"def", "text/plain"},
	{"deploy", "application/octet-stream"},
	{"disposition-notification", "message/disposition-notification"},
	{"dist", "application/octet-stream"},
	{"distz", "application/octet-stream"},
	{"dll", "application/octet-stream"},
	{"dmg", "application/octet-stream"},
	{"dms", "application/octet-stream"},
	{"doc", "application/msword"},
	{"dot", "application/msword"},
	{"drle", "image/dicom-rle"},
	{"dssc", "application/dssc+der"},
	{"dtd", "application/xml-dtd"},
	{"dump", "application/octet-stream"},
	{"ear", "application/java-archive"},
	{"ecma", "application/ecmascript"},
	{"elc", "application/octet-stream"},
	{"emf", "image/emf"},
	{"eml", "message/rfc822"},
	{"emma", "application/emma+xml"},
	{"eps", "application/postscript"},
	{"epub", "application/epub+zip"},
	{"es", "application/ecmascript"},
	{"exe", "application/octet-stream"},
	{"exi", "application/exi"},
	{"exr", "image/aces"},
	{"ez", "application/andrew-inset"},
	{"fits", "image/fits"},
	{"g3", "image/g3fax"},
	{"gbr", "application/rpki-ghostbusters"},
	{"geojson", "application/geo+json"},
	{"gif", "image/gif"},
	{"glb", "model/gltf-binary"},
	{"gltf", "model/gltf+json"},
	{"gml", "application/gml+xml"},
	{"gpx", "application/gpx+xml"},
	{"gram", "application/srgs"},
	{"grxml", "application/srgs+xml"},
	{"gxf", "application/gxf"},
	{"gz", "application/gzip"},
	{"h261", "video/h261"},
	{"h263", "video/h263"},
	{"h264", "video/h264"},
	{"heic", "image/heic"},
	{"heics", "image/heic-sequence"},
	{"heif", "image/heif"},
	{"heifs", "image/heif-sequence"},
	{"hjson", "application/hjson"},
	{"hlp", "application/winhlp"},
	{"hqx", "application/mac-binhex40"},
	{"htm", "text/html"},
	{"html", "text/html"},
	{"ics", "text/calendar"},
	{"ief", "image/ief"},
	{"ifb", "text/calendar"},
	{"iges", "model/iges"},
	{"igs", "model/iges"},
	{"img", "application/octet-stream"},
	{"in", "text/plain"},
	{"ini", "text/plain"},
	{"ink", "application/inkml+xml"},
	{"inkml", "application/inkml+xml"},
	{"ipfix", "application/ipfix"},
	{"iso", "application/octet-stream"},
	{"jade", "text/jade"},
	{"jar", "application/java-archive"},
	{"jls", "image/jls"},
	{"jp2", "image/jp2"},
	{"jpe", "image/jpeg"},
	{"jpeg", "image/jpeg"},
	{"jpf", "image/jpx"},
	{"jpg", "image/jpeg"},
	{"jpg2", "image/jp2"},
	{"jpgm", "video/jpm"},
	{"jpgv", "video/jpeg"},
	{"jpm", "image/jpm"},
	{"jpx", "image/jpx"},
	{"js", "application/javascript"},
	{"json", "application/json"},
	{"json5", "application/json5"},
	{"jsonld", "application/ld+json"},
	{"jsonml", "application/jsonml+json"},
	{"jsx", "text/jsx"},
	{"kar", "audio/midi"},
	{"ktx", "image/ktx"},
	{"less", "text/less"},
	{"list", "text/plain"},
	{"litcoffee", "text/coffeescript"},
	{"log", "text/plain"},
	{"lostxml", "application/lost+xml"},
	{"lrf", "application/octet-stream"},
	{"m1v", "video/mpeg"},
	{"m21", "application/mp21"},
	{"m2a", "audio/mpeg"},
	{"m2v", "video/mpeg"},
	{"m3a", "audio/mpeg"},
	{"m4a", "audio/mp4"},
	{"m4p", "application/mp4"},
	{"ma", "application/mathematica"},
	{"mads", "application/mads+xml"},
	{"man", "text/troff"},
	{"manifest", "text/cache-manifest"},
	{"map", "application/json"},
	{"mar", "application/octet-stream"},
	{"markdown", "text/markdown"},
	{"mathml", "application/mathml+xml"},
	{"mb", "application/mathematica"},
	{"mbox", "application/mbox"},
	{"md", "text/markdown"},
	{"me", "text/troff"},
	{"mesh", "model/mesh"},
	{"meta4", "application/metalink4+xml"},
	{"metalink", "application/metalink+xml"},
	{"mets", "application/mets+xml"},
	{"mft", "application/rpki-manifest"},
	{"mid", "audio/midi"},
	{"midi", "audio/midi"},
	{"mime", "message/rfc822"},
	{"mj2", "video/mj2"},
	{"mjp2", "video/mj2"},
	{"mjs", "application/javascript"},
	{"mml", "text/mathml"},
	{"mods", "application/mods+xml"},
	{"mov", "video/quicktime"},
	{"mp2", "audio/mpeg"},
	{"mp21", "application/mp21"},
	{"mp2a", "audio/mpeg"},
	{"mp3", "audio/mpeg"},
	{"mp4", "video/mp4"},
	{"mp4a", "audio/mp4"},
	{"mp4s", "application/mp4"},
	{"mp4v", "video/mp4"},
	{"mpd", "application/dash+xml"},
	{"mpe", "video/mpeg"},
	{"mpeg", "video/mpeg"},
	{"mpg", "video/mpeg"},
	{"mpg4", "video/mp4"},
	{"mpga", "audio/mpeg"},
	{"mrc", "application/marc"},
	{"mrcx", "application/marcxml+xml"},
	{"ms", "text/troff"},
	{"mscml", "application/mediaservercontrol+xml"},
	{"msh", "model/mesh"},
	{"msi", "application/octet-stream"},
	{"msm", "application/octet-stream"},
	{"msp", "application/octet-stream"},
	{"mxf", "application/mxf"},
	{"mxml", "application/xv+xml"},
	{"n3", "text/n3"},
	{"nb", "application/mathematica"},
	{"oda", "application/oda"},
	{"oga", "audio/ogg"},
	{"ogg", "audio/ogg"},
	{"ogv", "video/ogg"},
	{"ogx", "application/ogg"},
	{"omdoc", "application/omdoc+xml"},
	{"onepkg", "application/onenote"},
	{"onetmp", "application/onenote"},
	{"onetoc", "application/onenote"},
	{"onetoc2", "application/onenote"},
	{"opf", "application/oebps-package+xml"},
	{"otf", "font/otf"},
	{"owl", "application/rdf+xml"},
	{"oxps", "application/oxps"},
	{"p10", "application/pkcs10"},
	{"p7c", "application/pkcs7-mime"},
	{"p7m", "application/pkcs7-mime"},
	{"p7s", "application/pkcs7-signature"},
	{"p8", "application/pkcs8"},
	{"pdf", "application/pdf"},
	{"pfr", "application/font-tdpfr"},
	{"pgp", "application/pgp-encrypted"},
	{"pkg", "application/octet-stream"},
	{"pki", "application/pkixcmp"},
	{"pkipath", "application/pkix-pkipath"},
	{"pls", "application/pls+xml"},
	{"png", "image/png"},
	{"prf", "application/pics-rules"},
	{"ps", "application/postscript"},
	{"pskcxml", "application/pskc+xml"},
	{"qt", "video/quicktime"},
	{"raml", "application/raml+yaml"},
	{"rdf", "application/rdf+xml"},
	{"rif", "application/reginfo+xml"},
	{"rl", "application/resource-lists+xml"},
	{"rld", "application/resource-lists-diff+xml"},
	{"rmi", "audio/midi"},
	{"rnc", "application/relax-ng-compact-syntax"},
	{"rng", "application/xml"},
	{"roa", "application/rpki-roa"},
	{"roff", "text/troff"},
	{"rq", "application/sparql-query"},
	{"rs", "application/rls-services+xml"},
	{"rsd", "application/rsd+xml"},
	{"rss", "application/rss+xml"},
	{"rtf", "application/rtf"},
	{"rtx", "text/richtext"},
	{"s3m", "audio/s3m"},
	{"sbml", "application/sbml+xml"},
	{"scq", "application/scvp-cv-request"},
	{"scs", "application/scvp-cv-response"},
	{"sdp", "application/sdp"},
	{"ser", "application/java-serialized-object"},
	{"setpay", "application/set-payment-initiation"},
	{"setreg", "application/set-registration-initiation"},
	{"sgi", "image/sgi"},
	{"sgm", "text/sgml"},
	{"sgml", "text/sgml"},
	{"shex", "text/shex"},
	{"shf", "application/shf+xml"},
	{"shtml", "text/html"},
	{"sig", "application/pgp-signature"},
	{"sil", "audio/silk"},
	{"silo", "model/mesh"},
	{"slim", "text/slim"},
	{"slm", "text/slim"},
	{"smi", "application/smil+xml"},
	{"smil", "application/smil+xml"},
	{"snd", "audio/basic"},
	{"so", "application/octet-stream"},
	{"spp", "application/scvp-vp-response"},
	{"spq", "application/scvp-vp-request"},
	{"spx", "audio/ogg"},
	{"sru", "application/sru+xml"},
	{"srx", "application/sparql-results+xml"},
	{"ssdl", "application/ssdl+xml"},
	{"ssml", "application/ssml+xml"},
	{"stk", "application/hyperstudio"},
	{"styl", "text/stylus"},
	{"stylus", "text/stylus"},
	{"svg", "image/svg+xml"},
	{"svgz", "image/svg+xml"},
	{"t", "text/troff"},
	{"t38", "image/t38"},
	{"tei", "application/tei+xml"},
	{"teicorpus", "application/tei+xml"},
	{"text", "text/plain"},
	{"tfi", "application/thraud+xml"},
	{"tfx", "image/tiff-fx"},
	{"tif", "image/tiff"},
	{"tiff", "image/tiff"},
	{"tr", "text/troff"},
	{"ts", "video/mp2t"},
	{"tsd", "application/timestamped-data"},
	{"tsv", "text/tab-separated-values"},
	{"ttc", "font/collection"},
	{"ttf", "font/ttf"},
	{"ttl", "text/turtle"},
	{"txt", "text/plain"},
	{"u8dsn", "message/global-delivery-status"},
	{"u8hdr", "message/global-headers"},
	{"u8mdn", "message/global-disposition-notification"},
	{"u8msg", "message/global"},
	{"uri", "text/uri-list"},
	{"uris", "text/uri-list"},
	{"urls", "text/uri-list"},
	{"vcard", "text/vcard"},
	{"vrml", "model/vrml"},
	{"vtt", "text/vtt"},
	{"vxml", "application/voicexml+xml"},
	{"war", "application/java-archive"},
	{"wasm", "application/wasm"},
	{"wav", "audio/wav"},
	{"weba", "audio/webm"},
	{"webm", "video/webm"},
	{"webmanifest", "application/manifest+json"},
	{"webp", "image/webp"},
	{"wgt", "application/widget"},
	{"wmf", "image/wmf"},
	{"woff", "font/woff"},
	{"woff2", "font/woff2"},
	{"wrl", "model/vrml"},
	{"wsdl", "application/wsdl+xml"},
	{"wspolicy", "application/wspolicy+xml"},
	{"x3d", "model/x3d+xml"},
	{"x3db", "model/x3d+binary"},
	{"x3dbz", "model/x3d+binary"},
	{"x3dv", "model/x3d+vrml"},
	{"x3dvz", "model/x3d+vrml"},
	{"x3dz", "model/x3d+xml"},
	{"xaml", "application/xaml+xml"},
	{"xdf", "application/xcap-diff+xml"},
	{"xdssc", "application/dssc+xml"},
	{"xenc", "application/xenc+xml"},
	{"xer", "application/patch-ops-error+xml"},
	{"xht", "application/xhtml+xml"},
	{"xhtml", "application/xhtml+xml"},
	{"xhvml", "application/xv+xml"},
	{"xm", "audio/xm"},
	{"xml", "application/xml"},
	{"xop", "application/xop+xml"},
	{"xpl", "application/xproc+xml"},
	{"xsd", "application/xml"},
	{"xsl", "application/xml"},
	{"xslt", "application/xslt+xml"},
	{"xspf", "application/xspf+xml"},
	{"xvm", "application/xv+xml"},
	{"xvml", "application/xv+xml"},
	{"yaml", "text/yaml"},
	{"yang", "application/yang"},
	{"yin", "application/yin+xml"},
	{"yml", "text/yaml"},
	{"zip", "application/zip"}
};
