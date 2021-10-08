#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <map>

#include <utility> // for make pair

#include "User.hpp"
#define YEL "\033[33m" // make colors in other H file
#define END "\033[37m"

class Channel {
	public: // make private
		typedef std::map<std::string, User*> userMap; // del it
		
		private:
			Channel();

		public:
			Channel(std::string ch_name);
			~Channel();

		std::string _name; 						// name of channel
		std::map<std::string, User> _users;		// list of users // CHANGED
		std::map<std::string, User*> _chops;

		void addUser(User &_user); // ok
		void addChop(User &_user); // ok 
		void removeUser(std::string _nickname); // add him to ban list

		const std::map<std::string, User>	&getUsers() const; // ok
		const std::map<std::string, User*>	&getChops() const; // ok

		// new funcs //
		int _limit_users; // ok 
		std::vector<std::string> _ban_list; // ok 
		
		void channel_info(); //ok 

		// channel mode // ?? private ili net??
		// std::sttrig topic of channel 
		// void change topic // only for operator (Channel, new name)
		// void change name of channel 				(channel, new_channel name)

		// add to server remove channel
		// add to user -> get username 
		// username would be universal'nij //






};

#endif

