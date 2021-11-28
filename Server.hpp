#ifndef SERVER_HPP
# define SERVER_HPP

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <list>
#include <string>
#include <iostream>
#include <exception>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include "User.hpp"
#include "Channel.hpp"
#include "Command.hpp"

class Server
{
	public:
		typedef std::list<User *>::iterator		u_iterator;
		typedef std::list<Channel *>::iterator	c_iterator;

	private:
		fd_set					_master;
		int						_max;
		int						_listener;
		std::string				_password;
		std::list<User *>		_users;
		std::list<Channel *>	_channels;
		Server	&operator=(const Server &rhs);
		Server(const Server & other);
		

	public:
		Server(void);
		~Server(void);

		void					start(const std::string &port);

		void					addUser(void);
		User					*getSocketUser(int socket);
		User					*getUserNick(const std::string &str);
		std::list<User *>		&getUsers(void);
		void					deleteUser(const std::string &nick);
		void					deleteUser(int fd);
		void					addChannel(Channel *chann);
		Channel					*getChannelName(const std::string &str);
		std::list <Channel *>	&getChannels(void);
		void					deleteChannel(const std::string &name);

		void					setMax(int max);
		int						getMax(void) const;
		int						getListener(void) const;
		void					setMaster(fd_set set);
		fd_set					&getMaster(void);
		void					setPassword(const std::string &password);
		std::string				getPassword(void) const;
		bool					are_in_same_channels(int sender, int receiver);
};
#endif
