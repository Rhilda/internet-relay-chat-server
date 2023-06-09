#include "Channel.hpp"

Channel::Channel(std::string &name): _name(name) {}

Channel::~Channel() {}

Channel::Channel(const Channel & other)
{
	*this = other;
}

Channel 				&Channel::operator=(const Channel &rhs)
{
	this->_name = rhs._name;
	return (*this);
}

Channel					*Channel::clone(void) const
{
	return (new Channel(*this));
}

std::string				Channel::getTopic(void) const
{
	return (this->_topic);
}

std::string				Channel::getName(void) const
{
	return (this->_name);
}

std::string				Channel::getWhoTopicNick(void) const
{
	return (this->_whoTopicNick);
}
std::string				Channel::getWhoTopicSetat(void) const
{
	return (this->_whoTopicSetat);
}

int					Channel::getChanUsers() const
{
	return (this->_chanusers);
}

void					Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

void					Channel::setWhoTopicNick(std::string str)
{
	this->_whoTopicNick = str;
}

void					Channel::setWhoTopicSetat(std::string str)
{
	this->_whoTopicSetat = str;
}

void					Channel::setChanUsers(int n)
{
	(n > 0) ? this->_chanusers++ : this->_chanusers--;
}
