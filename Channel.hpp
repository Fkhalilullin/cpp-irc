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
	public: // make private
		typedef std::map<std::string, User*> userMap;

		
	private:
		Channel();

	public:
		Channel(std::string ch_name);
		~Channel();

		std::string _name; // ok
		std::string _topic; //  ok
		userMap _users; // ok 
		userMap _chops; // ok

		void addUser(User &_user); // ok
		void addChop(User &_user); // ok 
		void removeUser(std::string _nickname); // ok

		const userMap	&getUsers() const; // ok
		const userMap	&getChops() const; // ok

		// NEW FUNCS //
		int _limit_users; // ok // make it const
		std::vector<std::string> _ban_list; // ok
		
		void channel_info(); //ok 
		void change_topic(std::string new_topic); // ok // gde budet funk?
		// void change name of channel (channel, new_channel name) // na servere?

		// channel mode // ?? private ili net?? // dobavit' modi

		// add to server remove channel
		// add to user -> get username // -







};

#endif

