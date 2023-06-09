#pragma once

# include <list>
# include <string>
# include "Channel.hpp"

class Server;

class User
{
	public:
		typedef std::list<Channel *>::iterator	iterator;

	private:
		int						_socket;
		int						_hopcount;
		std::string				_nickname;
		std::string				_username;
		std::string				_realname;
		std::string				_hostname;
		bool					_password;
		bool					_isOP;
		bool					_isWelcomed;
		std::list<Channel*>		_joinedChannels;

		
	public:
		User(int fd);
		~User(void);
		User(void);
		User(const User & other);
		User	&operator=(const User &rhs);

		User					*clone(void) const;
		int						getSocket() const;
		int						getHopcount() const;
		std::string				getNickname() const;
		std::string				getUsername() const;
		std::string				getRealname() const;
		std::string				getHostname() const;
		bool					getIsOP() const;
		bool					isWelcomed() const;
		bool					getPassword() const;

		void					setHopcount(int hopcount);
		void					setNickname(std::string nickname);
		void					setIsOP(bool OP);
		void					setWelcomed(bool is_it);
		void					setUsername(std::string username);
		void					setRealname(std::string realname);
		void					setPassword(bool pass);

		void					addChannel(Channel *chann);
		Channel					*getChannelName(const std::string &str);
		std::list <Channel *>	&getListChannels(void);
		void					deleteChannel(const std::string &name);
		
		
		bool					is_in_channel(Channel *chan);
};
# include "Server.hpp"
