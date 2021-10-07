#ifndef USER_HPP
#define USER_HPP

#include <iostream>

class User {

	public: // private

		std::string _nickname;
		std::string _username; // the same
		std::string _realName;
		std::string _serverName; // the same
		bool		_password;
		bool		_logged; // orig nick
		int			_userSocket;



};


#endif
