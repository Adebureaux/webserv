#include "../includes/parsing.hpp"

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
	std::string srv = "server\n{";
	int i = 0;
	int ret = 0;

	while (i < content.length())
	{	
		if (content.substr(i, srv.length()) == srv)
			ret++;
		i++;
	}
	return (ret);
}

void get_elements(std::string content)
{
	int i = 0;
	int j = 0;
	bool bigBracket= 0;
	bool smallBracket= 0;
	std::string s1 = "";
	std::string s2 = "";

	while (i + 4 < content.length())
	{
		s1 = "";
		s2 = "";
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
		}
		if (!bigBracket)
		{
			while (content[i] && content[i] != '{')
				i++;
			bigBracket = 1;
			i++;
		}
		while (isspace(content[i]))
			i++;
		while (!isspace(content[i]))
		{
			s1 += content[i];
			i++;
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
		bloc.insert(std::pair<std::string, std::string>(s1, s2));
	}
}


int parser(char *file_name)
{
	std::string content;

	if (file_name)
		content = get_file_content(file_name);
	else
		return (FALSE);
	if (check_brackets(content) == FALSE)
		return (FALSE);
	nbrSrv = get_nbr_serv(content);
	get_elements(content);

	//
	std::multimap<std::string, std::string>::iterator 				it;
	for (it=bloc.begin(); it!=bloc.end(); it++)
	{
    	std::cout << (*it).first << " => " << (*it).second << '\n';
	}
	//

	return (TRUE);
}

int main (int ac, char **av)
{
	parser(av[1]);
	return (0);
}