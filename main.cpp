#include "Server.hpp"
#include <iostream>

std::string	*ft_av_parser(int ac, char **av)
{
	if (ac == 3)	
	{
		static std::string array[2];
		array[0] = av[1];
		array[1] = av[2];
		return (array);
	}
	if (ac == 4)	
	{
		static std::string array[5];
		int i = 0;
		char *aux = strtok(av[1], ":"); 
		while (aux != 0 && i < 5)
		{
			array[i++] = aux;
			aux = strtok(0, ":");
		}
		if (i != 3) 
			return (0);
		array[3] = av[2];
		array[4] = av[3];
		return (array);
	}
	return (0);
}

int main(int ac, char **av)
{
	std::string info;
	std::string *arg = ft_av_parser(ac, av);
	if (arg == 0)
	{
		std::cout << "error: ircserv: bad arguments" << std::endl;
		return (EXIT_FAILURE);
	}
	Server server;
	if (ac == 3)
	{
		server.setPassword(arg[1]);
		server.start(arg[0]);
	}
	else
		std::cout << "host:port_network:password_network is gone!" << std::endl;

	fd_set read_fds;
	while (1)
	{
		read_fds = server.getMaster();
		struct timeval tv;
		tv.tv_sec = 1;
    	tv.tv_usec = 0;

		if (select(server.getMax() + 1, &read_fds, NULL, NULL, &tv) == -1)
			throw std::runtime_error(strerror(errno));
		for (int i = 0; i <= server.getMax(); ++i)
		{
			if (FD_ISSET(i, &read_fds) == false)
				continue ;
			if (i == server.getListener())
				server.addUser();
			else
			{
				char buff[512];
				std::memset(buff, 0, sizeof buff);
				int nbytes;
				if ((nbytes = recv(i, buff, sizeof buff, 0)) <= 0)
				{
					if (nbytes != 0)
						throw std::runtime_error(strerror(errno));
					server.deleteUser(i);
				}
				else
				{					
					info += buff;
					// std::cout << info;
					// int len = info.length();
					// char  *str = (char *)info.c_str();
					// for (size_t i = 0; i < len; i++)
					// {
					// 	int c = str[i];
					// 	std::cout << c << ": " << str[i] << std::endl;
					// }
					
					User *client = server.getSocketUser(i);
					if (info.find("\r\n") != std::string::npos || info.find("\n") != std::string::npos)
					{
						//std::cout << "[" << info << "]" <<std::endl;
						Command cmd(info, server, *client);
						cmd.execute();
						info.clear();
					}
				}
			}
		}
	}
}
