#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>

#include "User.hpp"

class Channel {
	public: // make private
		typedef std::map<std::string, User*> userMap;

		std::string _name;
		userMap _users;
		userMap _chops;

		void addChop(User &_user);
		void addUser(User &_user);
		void removeUser(std::string _nickname);
		const userMap	&getUsers() const;
		const userMap	&getChops() const;



};

#endif

