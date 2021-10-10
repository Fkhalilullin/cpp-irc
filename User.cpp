#include "User.hpp"

User::User() {
	_password = false;
	_logged = false;
	_socket = -1;
}

User::User( const User &rhs ) : _nickname  (rhs._nickname  ),
                                _username  (rhs._username  ),
                                _realName  (rhs._realName  ),
                                _serverName(rhs._serverName),
                                _password  (rhs._password  ),
                                _logged    (rhs._logged    ),
                                _socket    (rhs._socket    )
{

}

User::~User() {

}

void User::setNickname(const std::string &nick)  {
	_nickname = nick;
}

void User::setUsername(const std::string &uname) {
	_username = uname;
}

void User::setRealName(const std::string &rname) {
	_realName = rname;
}

void User::setServerName(const std::string &sname) {
	_serverName = sname;
}

void User::switchPassword() {
	_password = !_password;
}

void User::switchLogged() {
	_logged = !_logged;
}

void User::setSocket(int socket) {
	_socket = socket;
}

int  User::getSocket() const
{
	return (_socket);
}

bool	User::isLogged   () const
{
	return (_logged);
}

bool	User::isPassworded() const
{
	return (_password);
}

std::string	User::getNickname () const
{
	return (_nickname);
}