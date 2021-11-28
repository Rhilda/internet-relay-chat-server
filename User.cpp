#include "User.hpp"

User::User(int fd)
: _socket(fd), _isOP(false), _isWelcomed(false), _password(false)
{
	return;
}

User::~User(void)
{
	_joinedChannels.clear();
	return;
}
User::User(const User &other)
{
	*this = other;
}
User		&User::operator=(const User &rhs)
{
	if (this == &rhs)
		return *this;
 	_socket = rhs._socket;
 	_hopcount = rhs._hopcount;
 	_nickname = rhs._nickname;
 	_username = rhs._username;
 	_realname = rhs._realname;
 	_password = rhs._password;
 	_hostname = rhs._hostname;
 	_isOP = rhs._isOP;
	_isWelcomed = rhs._isWelcomed;
	return (*this);
}

User					*User::clone(void) const
{
	return (new User(_socket));
}

int						User::getSocket(void) const
{
	return (this->_socket);
}
int						User::getHopcount(void) const
{
	return (this->_hopcount);
}
std::string				User::getNickname(void) const
{
	return (this->_nickname);
}
std::string				User::getUsername(void) const
{
	return (this->_username);
}
std::string				User::getHostname(void) const
{
	return (this->_hostname);
}
std::string				User::getRealname(void) const
{
	return (this->_realname);
}
bool					User::getIsOP(void) const
{
	return (this->_isOP);
}
bool					User::isWelcomed(void) const
{
	return (_isWelcomed);
}
bool					User::getPassword(void) const
{
	return (this->_password);
}

void					User::setHopcount(int hopcount)
{
	this->_hopcount = hopcount;
	return;
}
void					User::setNickname(std::string nickname)
{
	this->_nickname = nickname;
	return;
}
void					User::setUsername(std::string username)
{
	this->_username = username;
	return;
}
void					User::setRealname(std::string realname)
{
	this->_realname = realname;
	return;
}
void					User::setIsOP(bool OP)
{
	this->_isOP = OP;
	return;
}
void					User::setWelcomed(bool isit)
{
	_isWelcomed = isit;
}
void					User::setPassword(bool pass)
{
	this->_password = pass;
	return;
}


void					User::addChannel(Channel *chann)
{
	if (chann == nullptr)
		return ;
	for (iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		if ((*it)->getName() == chann->getName())
			return ;
	}
	chann->setChanUsers(1);
	_joinedChannels.push_back(chann);
}
Channel					*User::getChannelName(const std::string &str)
{
	for (iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		if ((*it)->getName() == str)
			return (*it);
	}
	return (nullptr);
}
std::list <Channel *>	&User::getListChannels(void)
{
	return (_joinedChannels);
}
void					User::deleteChannel(const std::string &name)
{
	for (iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			(*it)->setChanUsers(-1);
			_joinedChannels.erase(it);
			return ;
		}
		
	}
}

bool					User::is_in_channel(Channel *chan)
{
	if (chan == nullptr)
		return false;
	for (iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		if (*it == chan)
			return true;
	}
	return false;
}
