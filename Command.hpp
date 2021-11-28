#pragma once

# include <vector>
# include <locale>
# include <ctime>
# include <chrono>
# include <sstream>
# include <stdlib.h>

class Server;
class User;

class Command
{
	private:
		Server					&_server;
		User					&_commander;
		std::string				_prefix;
		std::string				_command;
		std::string				*_params;
		std::string				*_extra;
		int						_paramsNum;

		Command(void);
		Command(const Command &other);
	public:
		Command(std::string str, Server &server, User &commander);
		~Command(void);

		std::string		getCommand(void) const;
		std::string		*getParams() const;
		std::string		*getExtra() const;

		int							parseStr(std::string str);
		std::vector<std::string>	parseParam(std::string param);
		void						execute();
		void						numeric_reply(int key, std::string rply="", int socket=0);

		void				ftPASS();
		void				ftNICK();
		void				ftUSER();
		void				ftOPER();
		void				ftQUIT();
		void				ftJOIN();
		void				ftPART();
		void				ftTOPIC();
		void				ftNAMES();
		void				ftLIST();
		void				ftKICK();
		void				ftPRIVMSG();
		void				ftMODE();
};
# include "Server.hpp"
# include "User.hpp"
