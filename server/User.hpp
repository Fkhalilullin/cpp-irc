#ifndef USER_HPP
#define USER_HPP

#include <iostream>

class User {

	public: // private

		User();
		User( const User &rhs );
		~User();


		//private:
		std::string _nickname;
		std::string _username; // the same
		std::string _realName;
		std::string _serverName; // the same
		bool		_password;
		bool		_logged; // orig nick
		int			_socket;
		std::string _buffer;

	public:
		void	setNickname(const std::string &nick);
		void	setUsername(const std::string &uname);
		void	setRealName(const std::string &rname);
		void	setServerName(const std::string &sname);
		void	switchPassword();
		void	switchLogged();
		void	setSocket   (int socket);
		int		getSocket   () const;
		bool	isLogged    () const;
		bool	isPassworded() const;
		const std::string getNickname() const;

		void	clearBuffer();
		void	appendBuffer(const std::string &data);
		const std::string &getBuffer() const;
};


#endif
