#include "Command.hpp"

Command::Command(std::string str, Server &server, User &commander) : _server(server), _commander(commander)
{
	this->_params = new std::string[5];
	this->_extra = new std::string[5];
	if (!parseStr(str))
		return ;				
	return;
}

Command::~Command(void)
{
	delete[] this->_params;
	delete[] this->_extra;
	return ;
}

int		Command::parseStr(std::string str)
{
	if (str.empty())
		return (-1);						
	int pos1 = 0;
	int pos2 = str.find(" ");
	this->_command = str.substr(pos1, pos2);
	int i = 0;
	while (i < 5)			
	{
		pos1 = pos2 + 1;
		pos2 = str.find(" ", pos1);
		if (pos2 == std::string::npos || str[pos1] == ':')
			pos2 = str.find("\r\n", pos1);
		if (pos2 <= pos1)
			break;
		if (str[pos1] == ':')
			pos1++;
		this->_params[i++] = str.substr(pos1, pos2 - pos1);
	}
	this->_paramsNum = i;
	return (0);
}

std::vector<std::string>	Command::parseParam(std::string param)
{
	int pos1;
	int pos2 = -1;
	std::vector<std::string> rst;
	while (1)
	{
		pos1 = pos2 + 1;
		pos2 = param.find(",", pos1);
		if (pos2 == std::string::npos)
			pos2 = param.length();
		if (pos2 <= pos1)
			break;
		rst.push_back(param.substr(pos1, pos2 - pos1));
	}
	return (rst);
}

void		Command::execute(void)
{
	if (this->_command == "PASS")
		this->ftPASS();
	else if (this->_command == "PONG")
		return;
	else if (this->_commander.getPassword() || this->_server.getPassword().empty())
	{
		if (this->_command == "NICK")
			this->ftNICK();
		else if (this->_command == "USER" && !this->_commander.getNickname().empty())
			this->ftUSER();
		else if (!this->_commander.getUsername().empty())
		{
			if (this->_command == "OPER")
				this->ftOPER();
			else if (this->_command == "QUIT")
				this->ftQUIT();
			else if (this->_command == "JOIN")
				this->ftJOIN();
			else if (this->_command == "PART")
				this->ftPART();
			else if (this->_command == "TOPIC")
				this->ftTOPIC();
			else if (this->_command == "NAMES")
				this->ftNAMES();
			else if (this->_command == "LIST")
				this->ftLIST();
			else if (this->_command == "KICK")
				this->ftKICK();
			else if (this->_command == "PRIVMSG")
				this->ftPRIVMSG();
			else if (this->_command == "MODE")
				this->ftMODE();
			else
				this->numeric_reply(421);
		}
		else
			this->numeric_reply(421);
	}
	else
		this->numeric_reply(421);
	return ;
}

void			Command::ftPASS()
{
	if (this->_paramsNum == 0)
	{
		this->numeric_reply(431);
		return ;
	}

	if (this->_commander.getPassword())
	{
		this->numeric_reply(462);
		return ;
	}

	// if (this->_params[0] != this->_server.getPassword())
	// {
	// 	this->numeric_reply(464);
	// 	return ;
	// }

	this->_commander.setPassword(true);
	return;
}


void			Command::ftNICK()
{
	if (this->_paramsNum == 0)
	{
		this->numeric_reply(431);
		return ;
	}
	this->_extra[0] = this->_params[0];
	int i = -1;
	while (this->_params[0][++i])
	{
		if (!isalnum(this->_params[0][i]) && !strchr("-[]\\^{}", this->_params[0][i]))
		{
			this->numeric_reply(432);
			return ;
		}
		if (i > 8)
		{
			this->numeric_reply(432);
			return ;
		}
	}

	std::list<User *>::iterator it = this->_server.getUsers().begin();
	while (it != this->_server.getUsers().end())
	{
		if ((*it)->getNickname() == this->_params[0])
		{
			this->numeric_reply(433);
			return ;
		}
		it++;
	}
	std::string buff = ":" + _commander.getNickname() + " NICK " + _params[0] + "\r\n";
	this->_commander.setNickname(this->_params[0]);
	for (std::list<User *>::iterator u_it = _server.getUsers().begin();
		u_it != _server.getUsers().end(); ++u_it)
	{
		send((*u_it)->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
	}
	return ;
}


void		Command::ftUSER()
{
	if (this->_paramsNum < 4)
	{
		this->_extra[0] = this->_command;
		this->numeric_reply(461);
		return ;
	}
		
	if (!this->_commander.getUsername().empty())
	{
		this->numeric_reply(462);
		return ;
	}
	this->_commander.setUsername(this->_params[0]);
	this->_commander.setRealname(this->_params[3]);
	std::string ack = ": USER " + _params[0] + " 0 * " + _params[3] + "\r\n";
	send(_commander.getSocket(), ack.c_str(), strlen(ack.c_str()), 0);
	if (_commander.isWelcomed() == false)
	{
		this->numeric_reply(1);
		_commander.setWelcomed(true);
	}
	return ;
}


void		Command::ftOPER()
{
	if (this->_paramsNum < 2)
	{
		this->numeric_reply(461);
		return ;
	}
	if (this->_params[1] == "\"\"")
		this->_params[1] = "";
		
	if (this->_server.getPassword() != this->_params[1])
	{
		this->numeric_reply(464);
		return ;
	}

	std::list<User *>::iterator it;
	for (it = this->_server.getUsers().begin(); it != this->_server.getUsers().end(); ++it)
		if ((*it)->getNickname() == this->_params[0])
		{
			(*it)->setIsOP(true);
			this->numeric_reply(381, "", (*it)->getSocket());
		}
	return;
}


void		Command::ftQUIT()
{
	std::string buff = ":127.0.0.1 ERROR :You have disconnected\r\n";
	send(_commander.getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
	buff = ":" + _commander.getNickname() + " QUIT :" + _params[0]
		+ "\r\n";
	_server.deleteUser(_commander.getSocket());
	for (std::list<User*>::iterator it = _server.getUsers().begin(); it != _server.getUsers().end(); ++it)
	{
		send((*it)->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
	}
}

void		Command::ftJOIN()
{
	if (this->_paramsNum == 0 || this->_params[0].empty() || this->_params[0] == "#")
	{
		this->numeric_reply(461);
		return ;
	}
	int j = 0;
	Channel *aux;
	std::vector<std::string> targets = this->parseParam(_params[0]);
	std::vector<std::string>::iterator it = targets.begin();
	for (; it != targets.end(); ++it)
	{
		if (it->empty() == true || (it->at(0) != '&' && it->at(0) != '#'
			&& it->at(0) != '+' && it->at(0) != '!'))
			continue ;
		aux = this->_server.getChannelName(*it);
		if (!aux)
		{
			aux = new Channel(*it);
			this->_server.addChannel(aux);
			delete aux;
			this->_commander.addChannel(this->_server.getChannelName(*it));
		}
		else
		{
			if (_commander.is_in_channel(aux) == true)
				continue ;
			this->_commander.addChannel(aux);
		}
		std::string buff = ":" + _commander.getNickname() + " JOIN "
			+ *it + "\r\n";
		send(_commander.getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
		if (aux->getTopic().empty() == false)
		{
			this->_extra[0] = aux->getName();
			this->numeric_reply(332, aux->getTopic());
		}
		this->_extra[0] = "= " + *it;
		std::string rply;
		for (std::list<User *>::iterator u_iter = _server.getUsers().begin();
			u_iter != _server.getUsers().end(); ++u_iter)
		{
			if ((*u_iter)->is_in_channel(aux) == true)
				rply += (*u_iter)->getNickname() + " ";
		}
		this->numeric_reply(353, rply);
		this->_extra[0] = (*it);
		this->numeric_reply(366);
		buff = ":" + _commander.getNickname() + " JOIN " + *it + "\r\n";
		for (std::list<User *>::iterator u_iter = _server.getUsers().begin();
			u_iter != _server.getUsers().end(); ++u_iter)
		{
			if ((*u_iter)->getNickname() != _commander.getNickname() &&
				(*u_iter)->is_in_channel(aux) == true)
					send((*u_iter)->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
		}
	}
	return ;
}


void		Command::ftPART()
{
	if (this->_paramsNum < 1 || this->_params[0].empty())
	{
		this->numeric_reply(461);
		return;
	}
	std::vector<std::string> targets = this->parseParam(_params[0]);
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		this->_extra[0] = (*it);
		if (it->empty() == true)
			continue ;

		Channel *chan = _server.getChannelName(*it);
		if (chan == nullptr)
		{
			this->numeric_reply(403);
			continue ;
		}

		if (_commander.is_in_channel(chan) == false)
		{
			this->numeric_reply(442);
			continue ;
		}

		std::string buff = ":" + _commander.getNickname() + " PART "
			+ *it + " " + _params[1] + "\r\n";
		for (std::list<User *>::iterator it2 = _server.getUsers().begin();
			it2 != _server.getUsers().end(); ++it2)
			send((*it2)->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
		_commander.deleteChannel(*it);
	}
	return;
}


void		Command::ftTOPIC()	
{
	if (this->_paramsNum < 1 || this->_params[0].empty())
	{
		this->numeric_reply(461);
		return;
	}

	Channel *aux = this->_server.getChannelName(this->_params[0]);
	this->_extra[0] = this->_params[0];
	
	if (!aux)
		this->numeric_reply(403);

	else if (!this->_commander.is_in_channel(aux))
		this->numeric_reply(442);

	else if (this->_paramsNum == 1)		
	{
		if (aux->getTopic().empty())			
			this->numeric_reply(331);
		else									
		{
			this->numeric_reply(332, aux->getTopic());
			this->_extra[1] = aux->getWhoTopicNick();
			this->_extra[2] = aux->getWhoTopicSetat();
			this->numeric_reply(333);
		}
	}
	else if (this->_paramsNum >= 2)		
	{
			if (this->_params[1] == "\"\"")
				this->_params[1].clear();
			aux->setTopic(this->_params[1]);		
			aux->setWhoTopicNick(this->_commander.getNickname());
			std::stringstream time;
			time << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			aux->setWhoTopicSetat(time.str());
			for (std::list<User *>::iterator u_iter = this->_server.getUsers().begin();
			u_iter != this->_server.getUsers().end(); ++u_iter)
				if ((*u_iter)->is_in_channel(aux))
					this->numeric_reply(332, aux->getTopic(), (*u_iter)->getSocket());
	}
	return;
}


void		Command::ftNAMES()		
{
	if (this->_paramsNum < 1)
	{
		this->_extra[0] = "*";
		this->numeric_reply(366);
		return ;
	}
	std::list<User *> users = this->_server.getUsers();
	Channel *chan;

	std::vector<std::string> targets = this->parseParam(_params[0]);
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		if ((chan = _server.getChannelName(*it)) != nullptr)	
		{
			this->_extra[0] = "= " + (*it);
			std::string	rply;
			for (std::list<User *>::iterator u_iter = users.begin(); u_iter != users.end(); ++u_iter)
				if ((*u_iter)->is_in_channel(chan) == true)
				{
					rply += (*u_iter)->getNickname() + " ";
				}
			this->numeric_reply(353, rply);
			this->_extra[0] = (*it);
			this->numeric_reply(366);
			rply.clear();
		}
		else												
		{
			this->_extra[0] = (*it);
			this->numeric_reply(366);
		}
	}
	return ;
}

void		Command::ftMODE()
{
	this->_extra[0] = this->_params[0];
	this->_extra[1] = "b,k,l,imnpst";
	this->numeric_reply(324);
}

void		Command::ftLIST()	
{
	std::list<Channel *>::iterator it;

	this->numeric_reply(321);
	if (this->_paramsNum < 1)
		for (it = this->_server.getChannels().begin(); it != this->_server.getChannels().end(); ++it)
		{
			this->_extra[0] = (*it)->getName();
			this->_extra[1] = std::to_string((*it)->getChanUsers());
			this->numeric_reply(322, (*it)->getTopic());
		}
	else
	{
		std::vector<std::string> targets = this->parseParam(_params[0]);
		for (it = this->_server.getChannels().begin(); it != this->_server.getChannels().end(); ++it)
			for (std::vector<std::string>::iterator it2 = targets.begin(); it2 != targets.end(); ++it2)
				if ((*it)->getName() == *it2)
				{
					this->_extra[0] = (*it)->getName();
					this->_extra[1] = std::to_string((*it)->getChanUsers());
					this->numeric_reply(322, (*it)->getTopic());
				}
	}
	this->_extra[0].clear();
	this->_extra[1].clear();
	this->numeric_reply(323);
	return;
}


void		Command::ftKICK()
{
	if (_paramsNum < 2)
	{
		this->numeric_reply(461);
		return ;
	}
	if (_commander.getIsOP() == false)
	{
		this->numeric_reply(482);
		return ;
	}
	std::vector<std::string> from = this->parseParam(_params[0]);
	std::vector<std::string> targets = this->parseParam(_params[1]);
	if (from.size() != 1) 
		return ;
	Channel *aux = _server.getChannelName(from.at(0));
	if (aux == nullptr)
	{
		this->numeric_reply(403);
		return ;
	}
	if (_commander.is_in_channel(aux) == false)
	{
		this->numeric_reply(442);
		return ;
	}
	for (std::vector<std::string>::iterator it = targets.begin();
		it != targets.end(); ++it)
	{
		User *kicked = _server.getUserNick(*it);
		if (kicked->is_in_channel(aux) == false)
		{
			this->numeric_reply(441);
			continue ;
		}
		std::string buff = ":" + _commander.getNickname() + " KICK "
			+ from.at(0) + " " + *it + " " + _params[2] + "\r\n";
		kicked->deleteChannel(from.at(0));
		send(kicked->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
		buff = ":" + kicked->getNickname() + " PART "
			+ from.at(0) + " :Kicked by an operator!\r\n";
		send(kicked->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
		buff = ":" + _commander.getNickname() + " KICK "
			+ from.at(0) + " " + *it + " " + _params[2] + "\r\n";
		for (std::list<User *>::iterator it2 = _server.getUsers().begin();
			it2 != _server.getUsers().end(); ++it2)
			if ((*it2)->is_in_channel(aux) == true)
				send((*it2)->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
	}
}


void		Command::ftPRIVMSG()
{
	if (this->_paramsNum == 0)
	{
		this->numeric_reply(411);
		return;
	}
	if (this->_paramsNum == 1)
	{
		this->numeric_reply(412);
		return;
	}

	std::vector<std::string> targets = this->parseParam(this->_params[0]);

	std::string buff = ":" + _commander.getNickname() + " PRIVMSG";

	std::vector<std::string>::iterator it;
	for (it = targets.begin(); it != targets.end(); ++it)
	{
		buff += " " + *it + " :" + _params[1] + "\r\n";
		User *client;
		Channel *chan;

		if ((*it)[0] != '#' && (client = _server.getUserNick(*it)) != nullptr)
			send(client->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
		
		else if ((*it)[0] == '#' && (chan = _server.getChannelName(*it)) != nullptr)
		{
			if (_commander.is_in_channel(chan) == false)
			{
				this->_extra[0] = (*it);
				this->numeric_reply(404);
				continue ;
			}
			for (std::list<User *>::iterator u_it = _server.getUsers().begin();
				u_it != _server.getUsers().end(); ++u_it)
			{
				if ((*u_it)->getNickname() != _commander.getNickname() &&
					(*u_it)->is_in_channel(chan))
					send((*u_it)->getSocket(), buff.c_str(), strlen(buff.c_str()), 0);
			}
		}
		else
		{
			this->_extra[0] = (*it);
			this->numeric_reply(401);
		}
	}
	return;
}

std::string		Command::getCommand(void) const
{
	return (this->_command);
}
std::string		*Command::getParams(void) const
{
	return (this->_params);
}

std::string		*Command::getExtra(void) const
{
	return (this->_extra);
}

void			Command::numeric_reply(int key, std::string rply, int socket)
{
	std::string buff = ":127.0.0.1 " + std::to_string(key) + " "
		+ _commander.getNickname() + " ";
	if (_extra != nullptr)
	{
		int i = -1;
		while (_extra[++i].empty() == false)
			buff += _extra[i] + " ";
	}
	switch (key)		
	{
		case 1:		
			buff += ":Welcome to the IRC!";
			break;
		case 321:		
			buff += "Channel :Users  Name";
			break;
		case 322:		
			buff += ":" + rply;
			break;
		case 323:	
			buff += ":End of /LIST";
			break;
		case 324:
			break;
		case 331:		
			buff += ":No topic is set";
			break;
		case 332:		
			buff += ":" + rply;
			break;
		case 333:		
			break;
		case 353:		
			buff += ":" + rply;
			break;
		case 366:		
			buff += ":End of /NAMES list";
			break;
		case 381:		
			buff += ":You are now an IRC operator";
			break;
		case 401:	
			buff += ":No such nick/channel";
			break;
		case 403:	
			buff += ":No such channel";
			break;
		case 404:	
			buff += ":Cannot send to channel";
			break;
		case 407:		
			buff += ":Too many recipients/targets";
			break;
		case 411:		
			buff += ":No recipient given (" + _command + ")";
			break;
		case 412:		
			buff += ":No text to send";
			break;
		case 413:		
			buff += ":No toplevel domain specified";
			break;
		case 414:		
			buff += ":Wildcard in toplevel domain";
			break;
		case 421:		
			buff += this->_command + " :Unknown command";
			break;
		case 431:		
			buff += ":No nickname given";
			break;
		case 432:		
			buff += ":Erroneous nickname";
			break;
		case 433:		
			buff += ":Nickname is already in use";
			break;
		case 441:		
			buff += ":They aren't on that channel";
			break;
		case 442:		
			buff += ":You're not on that channel";
			break;
		case 461:		
			buff += this->_command + " :Not enough parameters";
			break;
		case 462:		
			buff += ":Unauthorized command (already registered)";
			break;
		case 464:		
			buff += ":Password incorrect";
			break;
		case 482:		
			buff += ":You're not channel operator";
			break;
		default:
			break;
	}
	buff += "\r\n";
	int nbytes = strlen(buff.c_str());
	if (!socket)
		socket = _commander.getSocket();
	if (send(socket, buff.c_str(), nbytes, 0) == -1)
		throw std::runtime_error(strerror(errno));
}
