#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <utility>
#include <vector>
#include <map>


#include "User.hpp"
#define YEL "\033[33m" // make colors in other H file
#define END "\033[37m"


class Channel {
	private: // make private
		typedef std::map<std::string, User*> userMap;
		std::string	_name;
		std::string	_topic;
		userMap		_users;
		userMap		_chops;

		
	private:
		Channel();

	public:
		Channel(std::string name);
		~Channel();

		void addUser(User &_user); // ok
		void addChop(User &_user); // ok 
		bool removeUser(std::string _nickname); // ok
		bool removeChop(std::string nick);
		// REMOVE USER != BAN USER -> make func ban user

		const std::string	& getName () const;
		const std::string	& getTopic() const;
		const userMap		& getUsers() const;
		const userMap		& getChops() const;
		
		bool				  setName ( std::string name  ); // checking the name for validity
		void				  setTopic( std::string topic );


		// NEW FUNCS //
		int _limit_users; // ok // make it const
		std::vector<std::string> _ban_list; // ok
		
		void channel_info(); //ok 
		void change_topic(std::string new_topic); // ok // gde budet funk?
		// void change name of channel (channel, new_channel name) // na servere?

		// channel mode // ?? private ili net?? // dobavit' modi

		// add to server remove channel
		// add to user -> get username // -

		// ПРОВЕРКА ИМеНИ КАНАЛА С СУЩЕСТВУЮЩИМИ ЮЗЕРАМИ

		std::string _pass;
		// make set_pass 




};

#endif

