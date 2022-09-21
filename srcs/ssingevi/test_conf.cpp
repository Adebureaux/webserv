#include "../../includes/Parser.hpp"
// #include "../../includes/Header.hpp"
#include "Header.hpp"
#include "Conf.hpp"


void print_location(std::vector<Location> locations)
{

	std::vector<Location>::const_iterator it = locations.begin();
	std::vector<Location>::const_iterator ite = locations.end();

	std::cout << C_G_CYAN;
	std::cout << "LOCATION" << std::endl;
	for(; it != ite; it++)
	{
		// 	bool						get_method;
		std::cout << "get_method : " << (it->get_method ? "true" : "false") << std::endl;
		// bool							post_method;
		std::cout << "post_method : " << (it->post_method ? "true" : "false")<< std::endl;
		// bool							delete_method;
		std::cout << "delete_method : " << (it->delete_method ? "true" : "false") << std::endl;
		// std::string					redirect;
		std::cout << "redirect : " << it->redirect << std::endl;
		// std::string					root;
		std::cout << "root : " << it->root << std::endl;
		// bool							autoindex;
		std::cout << "autoindex : " << (it->autoindex ? "true" : "false") << std::endl;
		// std::string					default_file;
		std::cout << "default_file : " << it->default_file <<std::endl;
		// std::string					CGI;
		std::cout << "CGI : " << it->CGI <<std::endl;
		// std::pair<bool, std::string>	upload;
		std::cout << "upload : " << (it->upload.first ? "true " + it->upload.second : "false") << std::endl;
		std::cout << std::endl;
	}
		std::cout << C_RES;
}

void print_error_pages(std::map<int, std::string> error_pages)
{
	std::map<int, std::string>::const_iterator it = error_pages.begin();
	std::map<int, std::string>::const_iterator ite = error_pages.end();

	std::cout << C_G_RED;
	std::cout << "error_pages : " << std::endl;
	for(; it != ite; it++)
	{
		std::cout << '\t' << it->first << " " << it->second << std::endl;
	}
	std::cout << C_RES << std::endl;
}

void print_server_block(std::map<int, std::map<std::string, Server_block> > &conf_value)
{
	std::map<int, std::map<std::string, Server_block> >::const_iterator it  = conf_value.begin();
	std::map<int, std::map<std::string, Server_block> >::const_iterator ite = conf_value.end();

	std::map<std::string, Server_block>::const_iterator it_m;
	std::map<std::string, Server_block>::const_iterator ite_m;
	Server_block										srv_bl;

	std::cout << C_G_GREEN;
	for (; it != ite; it++)
	{
		std::cout << "adwdawd" << std::endl;
		it_m = it->second.begin();
		ite_m = it->second.end();
		for (; it_m != ite_m; it_m++)
		{
			srv_bl = it_m->second;
			// int										port;
			std::cout << "port: " << srv_bl.port << std::endl;
			// std::string								address;
			std::cout << "address: " << srv_bl.address << std::endl; 
			// std::string								server_names;
			std::cout << "server_names: " << srv_bl.server_names << std::endl; 
			// bool									main;
			std::cout << "main: " << (srv_bl.main ? "true" : "false") <<std::endl; 
			// std::map<int, std::string>				error_pages;
			// std::cout << "error_pages:" << std::endl 
			print_error_pages(srv_bl.error_pages);
			std::cout << C_G_GREEN;
			// std::string								root;
			std::cout << "root: " << srv_bl.root <<  std::endl;
			// size_t									body_size;
			std::cout << "body_size: " << srv_bl.body_size << std::endl;
			// std::vector<Location>					locations;
			print_location(srv_bl.locations);
		}
	}
	std::cout << C_RES << std::endl;
}

int main(int ac, char **av)
{
	std::string conf_file;
	std::map<int, std::map<std::string, Server_block> > conf_value;

	if (ac == 2)
	{
		std::ifstream file(av[1]);
		std::stringstream iss;
		iss << file.rdbuf();
		conf_file = iss.str();
	}
	else
		return(1);
	
	Conf conf_class(conf_file);
	
	if (conf_class.is_valid())
		conf_value = conf_class.get_conf_map();
	else
	{
		std::cerr << conf_class.get_error_msg() << std::endl;
		return(1);
	}
	std::cout << "-------------" << std::endl;
	print_server_block(conf_value);
	std::cout << "-------------" << std::endl;
}