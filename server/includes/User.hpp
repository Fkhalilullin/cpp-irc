#ifndef USER_HPP
#define USER_HPP

#include <iostream>

class User {

	public:
		User();
		User( const User &rhs );
		~User();

	private:
		std::string _nickname;
		std::string _username;
		std::string _realName;
		std::string _serverName;
		std::string _buffer;
		std::string _sendBuffer;
		bool		_password;
		bool		_nick;
		bool		_user;
		bool		_logged;
		int			_socket;

	public:
		void	setNickname(const std::string &nick);
		void	setUsername(const std::string &uname);
		void	setRealName(const std::string &rname);
		void	setServerName(const std::string &sname);
		void	unablePassword();
		void	unableLogged();
		void	unableNick();
		void	unableUser();
		void	setSocket   (int socket);
		int		getSocket   () const;
		bool	isLogged    () const;
		bool	isPassworded() const;
		bool	isNick      () const;
		bool	isUser      () const;
		const std::string getNickname() const;

		void				clearBuffer		();
		void				appendBuffer	(const std::string &data);
		const std::string	&getBuffer		() const;
		void				setSendBuffer	(const std::string &data);
		const std::string	&getSendBuffer	() const;
};

#endif
