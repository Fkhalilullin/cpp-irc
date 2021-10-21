#include "Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

Channel::Channel(std::string name) {

		this->_name = name;
		this->_limit_users = 10;
		// this->_topic = "";
		// this->_pass = "123";

		std::cout << "channel \"" << this->_name <<  "\" created\n"; // del it
		// 1. nado li ogranicheniie na imya channela #$. and ....

}

void Channel::addUser(User &new_user) {

	// if user exists in channel
	// if (fine in ban list - >new_user._nickname) {
		// std::cout << "you are banned on channel" << std::endl;
		// return;
	// }
	// check limit

	if (_users.size() > this->_limit_users) {
		std::cout << "the limit of users is reached" << std::endl;
		return;
	}

	// check if nickname and username in channel already -> msg

	this->_users.insert(std::make_pair(new_user._nickname, &new_user));

	// send to all users that new user is connected
	// add user on chanel - > send him greeting maeesage // rules and other

}

void Channel::addChop(User &new_chop) {

	// if new_chop already in choops -> return

	// this->_chops.insert(std::make_pair(new_chop._nickname, &new_chop));
	this->_chops.push_back(new_chop);
}

void Channel::channel_info() {

	// add TOPIC CHANEL

	// add time of creating
	std::cout << "info about channel " << std::endl;
	std::cout << "channel name: " << this->_name << std::endl;

	std::cout << "list of users: " << std::endl;
	std::map<std::string, User*>::iterator iter = this->_users.begin();
	for (; iter != this->_users.end(); iter++) {
		std::cout << "nickname: " <<  iter->first << ", socket: " << iter->second->_socket << std::endl;
	}

	// list of chops
	// if (!_chops.empty()) {
	// 	std::cout << "list of chops in channel: "<< std::endl;
	// 	std::map<std::string, User*>::iterator itch = _chops.begin();
	// 	for (int i = 1; itch != _chops.end(); itch++, i++)
	// 		std::cout << YEL << i << ". " << itch->first << END << std::endl;
	// }

	// // list of banned users
	// if (!_ban_list.empty()) {
	// 	std::cout << "list of banned users: "<< std::endl;
	// 	std::vector<std::string>::iterator itban = _ban_list.begin();
	// 	for (int i = 1; itban != _ban_list.end(); itban++, i++)
	// 		std::cout << i << ". " << *itban << std::endl;
	// }

	// is it private?

}

const std::map<std::string, User*> &Channel::getUsers() const {

	return this->_users;
}

// const std::map<std::string, User*> &Channel::getChops() const {
const std::vector<User> &Channel::getChops() const {
	return this->_chops;
}

const std::string &Channel::getName() const {
	return this->_name;
}

const std::string &Channel::getTopic() const {
	return this->_topic;
}

bool	Channel::setName (std::string name)
{
	// checking the name for validity
	_name = name;
	return (true);
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

bool	Channel::removeUser(std::string rem_name)
{
	if (this->_users.erase(rem_name))
		return (true);
	// this->_chops.erase(rem_name);
	// this->_chops.erase(rem_name);

	// std::vector<User>::iterator us_it = this->_chops.begin();
	// for (; us_it != this->_chops.end(); us_it++) {
	// 	if (us_it->getNickname() == rem_name)
	// 		this->_chops.erase(us_it);
	// }

	return (false);

}

bool	Channel::removeChop(std::string nick)
{
	std::vector<User>::iterator us_it = this->_chops.begin();
	for (; us_it != this->_chops.end(); us_it++) {
		if (us_it->getNickname() == nick) {
			this->_chops.erase(us_it);
			return true;
		}
	}
	return false;


	// if (this->_chops.erase(nick))
	// 	return (true);
	// return (false);
}

std::vector<User>::const_iterator const Channel::getChop(std::string const &nick) const {

	std::vector<User>::const_iterator us_it = this->_chops.begin();
	for (; us_it != this->_chops.end(); us_it++) {
		if (us_it->getNickname() == nick) {
			return us_it;
		}
	}
	return us_it;
}

void Channel::change_topic(std::string new_topic) {

	this->_topic = new_topic;
}
