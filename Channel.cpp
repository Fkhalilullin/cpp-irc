#include "Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

Channel::Channel(std::string ch_name) {

		this->_name = ch_name;
		this->_limit_users = 10;
		std::cout << "channel \"" << this->_name <<  "\" created\n"; // del it
		// 1. nado li ogranicheniie na imya channela #$. and ....

}

void Channel::addUser(User &new_user) {

	// if user exists in channel
	// if (fine in ban list - >new_user._username) {
		// std::cout << "you are banned on channel" << std::endl;
		// return;
	// }
	// check limit

	if (_users.size() > this->_limit_users) {
		std::cout << "the limit of users is reached" << std::endl;
		return;
	}

	// check if nickname and username in channel already -> msg

	this->_users.insert(std::make_pair(new_user._nickname, new_user));
	
	// send to all users that new user is connected 
	// add user on chanel - > send him greeting maeesage // rules and other

}

void Channel::addChop(User &new_chop) {
	
	// if new_chop already in choops -> return

	this->_chops.insert(std::make_pair(new_chop._nickname, &new_chop));

}

void Channel::channel_info() {

	// add time of creating
	std::cout << "info about channel " << std::endl;
	std::cout << "channel name: " << this->_name << std::endl;
	
	std::cout << "list of users: " << std::endl;
	std::map<std::string, User>::iterator iter = this->_users.begin();
	for (; iter != this->_users.end(); iter++) {
		std::cout << "nickname: " <<  iter->first << " socket: " << iter->second._socket << std::endl;
	}
	
	// list of chops
	if (!_chops.empty()) {
		std::cout << "list of chops in channel: "<< std::endl;
		std::map<std::string, User*>::iterator itch = _chops.begin();
		for (int i = 1; itch != _chops.end(); itch++, i++)
			std::cout << YEL << i << ". " << itch->first << END << std::endl;
	}

	// list of banned users
	if (!_ban_list.empty()) {
		std::cout << "list of banned users: "<< std::endl;
		std::vector<std::string>::iterator itban = _ban_list.begin();
		for (int i = 1; itban != _ban_list.end(); itban++, i++)
			std::cout << i << ". " << *itban << std::endl;
	}

	// is it private?

}

const std::map<std::string, User> &Channel::getUsers() const {
	
	return this->_users;
}

const std::map<std::string, User*> &Channel::getChops() const {

	return this->_chops;
}

void Channel::removeUser(std::string rem_name) { // add here username //  add User //

	this->_ban_list.push_back(rem_name);
	
	this->_users.erase(rem_name);
	this->_chops.erase(rem_name); // if he wasnt a chop?
	std::cout << rem_name << " was banned and added to black list" << std::endl; // add here red color


}