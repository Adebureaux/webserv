#include "../includes/parsing.hpp"

std::string getALocationValue(int serverIndex, std::string key)
{
	std::string ret = "";
	std::multimap<std::string, std::string>::iterator 				it1;
	it1 = locations[serverIndex].begin();
	while (it1 != locations[serverIndex].end())
	{
		if ((*it1).first == key)
		{
			ret += (*it1).second;
			ret += '\n';
		}
		it1++;
	}
	return (ret);
}

std::string getAServerValue(int serverIndex, std::string key)
{
	std::string ret = "";
	std::multimap<std::string, std::string>::iterator 				it1;
	it1 = servers[serverIndex].begin();
	while (it1 != servers[serverIndex].end())
	{
		if ((*it1).first == key)
		{
			ret += (*it1).second;
			ret += '\n';
		}
		it1++;
	}
	return (ret);
}

int check_file_name(std::string file_name)
{
	int i = file_name.length() - 1;
	if (file_name.length() < 6 || file_name[i - 4] != '.' || file_name[i - 3] != 'c'
		|| file_name[i - 2] != 'o' || file_name[i - 1] != 'n' || file_name[i] != 'f')
		return (0);
	return (1);
}

std::string get_file_content(std::string file_name)
{
	std::string content;
	std::string line;
	std::ifstream infile;
	char c;
	infile.open(file_name);
	if (!infile)
	{
		std::cerr << "Error: file could not be opened" << std::endl;
		infile.close();
		exit(1);
	}
	while (std::getline(infile, line))
	{
		content += line;
		content += '\n';
	}
	infile.close();
	return (content);
}

int check_brackets(std::string content)
{
	int opn = 0;
	int clos = 0;
	int i = 0;

	while (content[i])
	{
		if (content[i] == '{')
			opn++;
		else if (content[i] == '}')
			clos++;
		i++;
	}
	if (opn != clos)
		return (FALSE);
	return (TRUE);
}

int get_nbr_serv(std::string content)
{
	std::string srv = "server";
	int i = 0;
	int ret = 0;

	while (i < content.length())
	{
		if (content.substr(i, srv.length()) == srv)
		{
			i += srv.length();
			while (i < content.length() && isspace(content[i]))
				i++;
			if (content[i] == '{')
				ret++;
		}
		i++;
	}
	return (ret);
}

void get_elements(std::string content, int *index, int serverIndex)
{
	int i = *index;
	int location = 0;
	bool bigBracket= 0;
	bool smallBracket= 0;
	std::string s1 = "";
	std::string s2 = "";
	std::string temp = "";

	while (i < content.length())
	{
		s1 = "";
		s2 = "";
		if (!bigBracket && !smallBracket)
		{
			while (content[i] && content[i] != '{')
				i++;
			bigBracket = 1;
			i++;
		}
		if (content[i] == '{')
		{
			smallBracket = 1;
			i++;
		}
		if (content[i] == '}' && smallBracket)
		{
			smallBracket = 0;
			i++;
		}
		while (isspace(content[i]))
			i++;
		if (content[i] == '}' && !smallBracket)
		{
			bigBracket = 0;
			i++;
			*index = i;
			locations.insert(std::pair<int, std::multimap<std::string, std::string> >(serverIndex, lBloc));
			lBloc.clear();
			break ;
		}
		while (isspace(content[i]))
			i++;
		while (!isspace(content[i]))
		{
			s1 += content[i];
			i++;
		}
		if (s1 == "error_page")
		{
			s1 += " ";
			i++;
			while (isdigit(content[i]))
			{
				s1 += content[i];
				i++;
			}
		}
		while (isspace(content[i]))
			i++;
		while (!isspace(content[i]) && s1 != "http_methods")
		{
			s2 += content[i];
			i++;
		}
		if (s1 == "http_methods")
		{
			while (content[i] != '\n')
			{
				s2 += content[i];
				i++;
			}
		}
		while (isspace(content[i]))
			i++;
		if (s1 == "location")
			location++;
		if (location != 0)
		{
			temp += std::to_string(location);
			temp += s1;
			s1 = temp;
			temp.clear();
			lBloc.insert(std::pair<std::string, std::string>(s1, s2));
		}
		else
			sBloc.insert(std::pair<std::string, std::string>(s1, s2));
	}
}

int check_only_different_ports()
{
	std::vector<int> nums;
	std::multimap<std::string, std::string>::iterator 				it1;
	std::map<int, std::multimap<std::string, std::string> >::iterator 				it2;
	it2 = servers.begin();
	while (it2 != servers.end())
	{
		it1 = it2->second.begin();
		while (it1 != it2->second.end())
		{
    		if ((*it1).first == "listen")
				nums.push_back(atoi(((*it1).second).c_str()));
			it1++;
		}
		it2++;
	}
	std::vector<int>::iterator it;
	std::vector<int>::iterator ite;
	it = nums.begin();
	while (it != nums.end())
	{
		ite = it + 1;
		while (ite != nums.end())
		{
			if ((*it) == (*ite))
				return (0);
			ite++;
		}
		it++;
	}
	return (1);
}

int check_server_names()
{
	std::vector<std::string> names;
	std::multimap<std::string, std::string>::iterator 				it1;
	std::map<int, std::multimap<std::string, std::string> >::iterator 				it2;
	it2 = servers.begin();
	while (it2 != servers.end())
	{
		it1 = it2->second.begin();
		while (it1 != it2->second.end())
		{
    		if ((*it1).first == "server_name")
				names.push_back((*it1).second);
			it1++;
		}
		it2++;
	}
	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator ite;
	it = names.begin();
	while (it != names.end())
	{
		ite = it + 1;
		while (ite != names.end())
		{
			if ((*it) == (*ite))
				return (0);
			ite++;
		}
		it++;
	}
	return (1);
}

int parser(char *file_name)
{
	std::string content;
	int index = 0;
	int i = 0;


	if (file_name && check_file_name(file_name))
		content = get_file_content(file_name);
	else
		return (FALSE);
	if (check_brackets(content) == FALSE)
		return (FALSE);
	nbrSrv = get_nbr_serv(content);
	while (i < nbrSrv)
	{
		get_elements(content, &index, i);
		servers.insert(std::pair<int, std::multimap<std::string, std::string> >(i, sBloc));
		sBloc.clear();
		i++;
	}

	if (!check_only_different_ports())
		return (FALSE);
	if (!check_server_names())
		return (FALSE);

	//AFFICHAGE debut
	std::multimap<std::string, std::string>::iterator 				it1;
	std::map<int, std::multimap<std::string, std::string> >::iterator 				it2;
	std::cout << "servers :\n" << std::endl;
	it2 = servers.begin();
	while (it2 != servers.end())
	{
		it1 = it2->second.begin();
		while (it1 != it2->second.end())
		{
    		std::cout << (*it1).first << " => " << (*it1).second << '\n';
			it1++;
		}
		std::cout << std::endl;
		it2++;
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "locations :\n" << std::endl;
	std::multimap<std::string, std::string>::iterator 				it3;
	std::multimap<int, std::multimap<std::string, std::string> >::iterator 				it4;
	it4 = locations.begin();
	while (it4 != locations.end())
	{
		it3 = it4->second.begin();
		while (it3 != it4->second.end())
		{
    		std::cout << (*it3).first << " => " << (*it3).second << '\n';
			it3++;
		}
		std::cout << std::endl;
		it4++;
	}
	//AFFICHAGE fin

	std::cout << "test get serv value : " << getAServerValue(1, "listen") << std::endl;
	std::cout << "test get loc value : " << getALocationValue(0, "2default") << std::endl;
	std::cout << "test get loc value : " << getALocationValue(0, "2root") << std::endl;
	
	return (TRUE);
}

int main (int ac, char **av)
{
	parser(av[1]);
	return (0);
}