#include "../includes/Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

Channel::Channel(std::string name) : _name(name),
									 _limit_users(10) {}

void Channel::addUser(User &new_user)
{

	if (_users.size() > this->_limit_users)
	{
		std::cout << "the limit of users is reached" << std::endl;
		return;
	}

	this->_users.insert(std::make_pair(new_user.getNickname(), &new_user));
}

void Channel::addChop(User &new_chop)
{
	this->_chops.push_back(new_chop);
}

const std::map<std::string, User *> &Channel::getUsers() const
{
	return this->_users;
}

const std::vector<User> &Channel::getChops() const
{
	return this->_chops;
}

const std::string &Channel::getName() const
{
	return this->_name;
}

const std::string &Channel::getTopic() const
{
	return this->_topic;
}

bool Channel::setName(std::string name)
{
	_name = name;
	return (true);
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

bool Channel::removeUser(std::string rem_name)
{
	if (this->_users.erase(rem_name))
		return (true);
	return (false);
}

bool Channel::removeChop(std::string nick)
{
	std::vector<User>::iterator us_it = this->_chops.begin();
	for (; us_it != this->_chops.end(); us_it++)
	{
		if (us_it->getNickname() == nick)
		{
			this->_chops.erase(us_it);
			return true;
		}
	}
	return false;
}

std::vector<User>::const_iterator const Channel::getChop(std::string const &nick) const
{
	std::vector<User>::const_iterator us_it = this->_chops.begin();
	for (; us_it != this->_chops.end(); us_it++)
	{
		if (us_it->getNickname() == nick)
		{
			return us_it;
		}
	}
	return us_it;
}

void Channel::changeTopic(std::string new_topic)
{
	this->_topic = new_topic;
}

const std::string &Channel::getPass() const
{
	return this->_pass;
}

void Channel::setPass(std::string pass)
{
	this->_pass = pass;
}
