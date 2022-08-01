#include "../includes/parsing.hpp"

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
	infile.open(file_name.c_str());
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
	if (opn != clos || opn == 0 || clos == 0)
		return (FALSE);
	return (TRUE);
}

int get_nbr_serv(std::string content)
{
	std::string srv = "server";
	unsigned int i = 0;
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

void nginx::get_elements(std::string content, int *index)
{
	unsigned int i = *index;
	bool bigBracket = 0;
	bool smallBracket = 0;
	std::string s1 = "";
	std::string s2 = "";
	std::string currLoc = "";
	int srvr = 0;
	bool errorPage = 0;


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
		if (srvr == 0)
			srvr = 1;
		if (content[i] == '{')
		{
			smallBracket = 1;
			i++;
		}
		if (content[i] == '}' && smallBracket)
		{
			smallBracket = 0;
			servBloc.insert(std::make_pair(currLoc, innerBloc));
			innerBloc.clear();
			i++;
		}
		while (isspace(content[i]))
			i++;
		if (content[i] == '}' && !smallBracket)
		{
			bigBracket = 0;
			i++;
			*index = i;
			break ;
		}
		while (isspace(content[i]))
			i++;
		while (!isspace(content[i]))
		{
			s1 += content[i];
			i++;
		}
		if (!s1.empty() && s1 == "error_page")
		{
			i++;
			if (errorPage == 0)
			{
				innerBloc.insert(std::pair< std::string, std::vector< std::string > >(s1, std::vector< std::string >()));
				errorPage = 1;
			}
			while (isdigit(content[i]))
			{
				s2 += content[i];
				i++;
			}
			while (isspace(content[i]))
			{
				s2 += content[i];
				i++;
			}
		}
		while (isspace(content[i]))
			i++;
		while (!isspace(content[i]) && !s1.empty() && s1 != "http_methods")
		{
			s2 += content[i];
			i++;
		}
		if (!s1.empty() && s1 == "http_methods")
		{
			while (content[i] != '\n')
			{
				s2 += content[i];
				i++;
			}
		}
		while (isspace(content[i]))
			i++;
		//std::cout << s1 << "    |     " << s2 << std::endl;
		if (!s2.empty() && s2[s2.length()-1] == ';')
    		s2.erase(s2.length()-1);
		if (!s1.empty() && s1 == "autoindex")
		{
			if (!s2.empty() && s2 == "on")
				s2 = "1";
			else if (!s2.empty() && s2 == "off")
				s2 = "0";
		}
		if (!s1.empty() && s1 == "location" && srvr == 1)
		{
			srvr = 2;
			servBloc.insert(std::make_pair("server", innerBloc));
			innerBloc.clear();
		}
		if (srvr == 2)
		{
			if (!s1.empty() && s1 == "location")
			{
				s1 += " ";
				s1 += s2;
				currLoc = s1;
			}
			if (s1 != currLoc)
			{
				innerBloc.insert(std::pair< std::string, std::vector< std::string > >(s1, std::vector< std::string >()));
				innerBloc[s1].push_back(s2);
			}
		}
		if (!s1.empty() && srvr == 1 && s1 != "server")
		{
			if (!s1.empty() && s1 != "error_page")
				innerBloc.insert(std::pair< std::string, std::vector< std::string > >(s1, std::vector< std::string >()));
			innerBloc[s1].push_back(s2);
		}
	}
}

int nginx::check_only_different_ports()
{
	std::vector<int> nums;
	unsigned int i;
    unsigned int j;
	unsigned int check;

	i = 0;
	j = 0;
	check = 0;
    while (i < conf.size())
    {
        for (std::map< std::string, std::map< std::string, std::vector< std::string > > >::iterator it = conf.data()[i].begin(); it != conf.data()[i].end(); it++)
        {
            for (std::map< std::string, std::vector< std::string > >::iterator itt = conf.data()[i][it->first].begin(); itt != conf.data()[i][it->first].end(); itt++)
            {
                while (j < itt->second.size())
                {
					if ((*itt).first == "listen")
						nums.push_back(atoi(((*itt).second[j]).c_str()));
                    j++;
                }
                j = 0;
            }
        }
        i++;
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
			{
				return (0);
			}
			ite++;
		}
		it++;
		check++;
	}
	if (check == 0)
		return (0);
	return (1);
}

int nginx::check_server_names()
{
	std::vector<std::string> names;
	unsigned int i;
    unsigned int j;

	i = 0;
	j = 0;
    while (i < conf.size())
    {
        for (std::map< std::string, std::map< std::string, std::vector< std::string > > >::iterator it = conf.data()[i].begin(); it != conf.data()[i].end(); it++)
        {
            for (std::map< std::string, std::vector< std::string > >::iterator itt = conf.data()[i][it->first].begin(); itt != conf.data()[i][it->first].end(); itt++)
            {
                while (j < itt->second.size())
                {
					if ((*itt).first == "server_name")
						names.push_back((*itt).second[j]);
                    j++;
                }
                j = 0;
            }
        }
        i++;
    }
	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator ite;
	it = names.begin();
	while (it != names.end())
	{
		ite = it + 1;
		while (ite != names.end())
		{
			//std::cout << (*it) << "   " << (*ite) << std::endl;
			if ((*it) == (*ite))
				return (0);
			ite++;
		}
		it++;
	}
	return (1);
}

nginx parser(char *file_name)
{
	nginx srv;
	std::string content;
	int index = 0;
	int i = 0;


	if (file_name && check_file_name(file_name))
		content = get_file_content(file_name);
	else
	{
		srv.parsing_ok = FALSE;
		return (srv);
	}
	if (check_brackets(content) == FALSE)
	{
		srv.parsing_ok = FALSE;
		return (srv);
	}
	srv.nbrSrv = get_nbr_serv(content);

	while (i < srv.nbrSrv)
	{
		srv.get_elements(content, &index);
		//servers.insert(std::pair<int, std::map<std::string, std::string> >(i, innerBloc));
		srv.conf.push_back(srv.servBloc);
		srv.servBloc.clear();
		i++;
	}
	if (!srv.check_only_different_ports())
	{
		srv.parsing_ok = FALSE;	
		return (srv);
	}

	if (!srv.check_server_names())
	{
		srv.parsing_ok = FALSE;	
		return (srv);
	}
	srv.parsing_ok = TRUE;
	//srv.printMap();
	return (srv);
}

void nginx::printMap()
{
    unsigned int i;
    unsigned int j;

	i = 0;
	j = 0;
    while (i < conf.size())
    {
        std::cout << "\nServer number : " << (i + 1) << "\n.................................................................................\n";
        for (std::map< std::string, std::map< std::string, std::vector< std::string > > >::iterator it = conf.data()[i].begin(); it != conf.data()[i].end(); it++)
        {
            std::cout << "---------\nCurrent inner server bloc is : " << it->first << "\n";
            for (std::map< std::string, std::vector< std::string > >::iterator itt = conf.data()[i][it->first].begin(); itt != conf.data()[i][it->first].end(); itt++)
            {
                std::cout << "\nCurrent sub KEY is : " << itt->first << std::endl;;
                while (j < itt->second.size())
                {
                    std::cout << "  Associated to VALUE : " << itt->second[j] <<std::endl;
                    j++;
                }
                j = 0;
            }
            std::cout << std::endl;
        }
        std::cout << "\n.................................................................................\n";
        std::cout << std::endl;
        i++;
    }
}