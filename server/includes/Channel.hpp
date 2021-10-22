#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <utility>
#include <vector>
#include <map>

#include "User.hpp"

class Channel {
	private:
		typedef std::map<std::string, User*> userMap;
		std::string				_name;
		std::string				_topic;
		std::string				_pass;
		userMap					_users;
		std::vector<User>		_chops;
		size_t					_limit_users;

	private:
		Channel();

	public:
		Channel(std::string name);
		~Channel();

		void addUser(User &_user);
		void addChop(User &_user);
		bool removeChop(std::string nick);
		bool removeUser(std::string _nickname);
		void changeTopic(std::string new_topic);

		const std::string			& getName () const;
		const std::string			& getTopic() const;
		const userMap				& getUsers() const;
		const std::vector<User>		& getChops() const;
		const std::string			& getPass () const;
		std::vector<User>::const_iterator const getChop(std::string const &nick) const;

		bool				  setName ( std::string name  );
		void				  setTopic( std::string topic );
		void				  setPass ( std::string pass  );
};

#endif

