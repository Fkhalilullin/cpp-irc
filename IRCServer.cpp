#include "IRCServer.hpp"
#include <string>

IRCServer::IRCServer(unsigned int port, std::string pass) :
	_port(port),
	_password(pass),
    _delimeter("\n")
{
    char    hostname[30];

    memset(hostname, 0, sizeof(hostname));
    gethostbyname(hostname);
    _hostname = hostname;

    _serverAdress.sin_family = AF_INET;
	_serverAdress.sin_addr.s_addr = INADDR_ANY;
	_serverAdress.sin_port = htons(this->_port);

		// zanulentie fd set 

}

IRCServer::~IRCServer() {}

void IRCServer::_accept()
{
    struct sockaddr_in temp;

    socklen_t socklen = sizeof(struct sockaddr_in); // temp
    int new_fd = accept(_listener, (struct sockaddr*)&temp, &socklen); // reinpretet cast ? // eto new client socket
    if (new_fd == -1)
        throw std::invalid_argument(strerror(errno));
    _addUser(new_fd);
    if (_max_fd < new_fd)
        _max_fd = new_fd;
    FD_SET(new_fd, &_client_fds);
    std::cout << GRE << "New connetion on socket : " << new_fd << END << std::endl;
}

void IRCServer::start()
{
	_listener = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
	if (_listener < 0)
		throw std::invalid_argument(strerror(errno));
    _max_fd = _listener;

    fcntl(_listener, F_SETFL, O_NONBLOCK);

    int yes = 1;
    setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	int b = bind(_listener, (struct sockaddr*)&_serverAdress, sizeof(_serverAdress));
	if (b < 0)
		throw std::invalid_argument(strerror(errno));

    int l = listen(_listener, 10);
	if (l < 0)
		throw std::invalid_argument(strerror(errno));

	FD_SET(_listener, &_client_fds);
	_max_fd = _listener;
	fd_set  select_fds; // just for copy
	select_fds = _client_fds;

	while (select(_max_fd + 1, &select_fds, NULL, NULL, NULL) != -1)
    {
		for (int i = 3; i < _max_fd + 1; i++)
        {
			if (!FD_ISSET(i, &select_fds) || i == _listener)
                continue ;
            std::string buf;

            try
            {
                _recv(i, buf);
            }
            catch (const std::exception& e)
            {
                close(i);
                FD_CLR(i, &this->_client_fds);
                _removeUser(i);
            }

            
            std::multimap<std::string, User>::iterator  it;
            
            it = _users.begin();
            while (it != _users.end() && it->second.getSocket() != i)
                it++;
            if (it != _users.end())
            {
                Message msg(buf, it->second);

                _CAP (msg, it->second);
                _PASS(msg, it->second);
                _PING(msg, it->second);
                if (it->second.isPassworded())
                {
                    _NICK(msg, it->second);
                    _USER(msg, it->second);
                }
                // _send(i, std::string(":nforce2 PRIVMSG #chan2 :hello!"));
            }
            std::cout << "Number of users : " 
                        << _users.size() << std::endl;
		}
        if (FD_ISSET(_listener, &select_fds))
            _accept();
		select_fds = _client_fds;
	}
    FD_ZERO(&select_fds );
    FD_ZERO(&_client_fds);
    throw std::invalid_argument(strerror(errno));
}

bool    IRCServer::_recv( int sockfd, std::string &buf ) const
{
    char    c_buf[512];
	int     bytesLeft;
	int     bytes = 1;

    buf.clear();
    while (buf.find(_delimeter) == std::string::npos
                            && buf.length() < sizeof(c_buf))
    {
        memset(c_buf, 0, sizeof(c_buf));
        bytes = recv(sockfd, c_buf, sizeof(c_buf), MSG_PEEK);
        if (bytes < 0)
        {
            std::cerr << RED << strerror(errno) << END;
            throw std::exception();
        }
        if (bytes == 0)
            throw std::exception();

        bytesLeft = std::string(c_buf).find(_delimeter);
        if (bytesLeft == std::string::npos)
            bytesLeft = bytes;
        else
            bytesLeft += _delimeter.length();
        while (bytesLeft > 0)
        {
            memset(c_buf, 0, sizeof(c_buf));
            bytes = recv(sockfd, c_buf, bytesLeft, 0);
            if (bytes < 0)
            {
                std::cerr << RED << strerror(errno) << END;
                throw std::exception();
            }
            if (bytes == 0)
                throw std::exception();
            bytesLeft -= bytes;
            buf       += c_buf;
        }
    }
    std::cout << GRE << "▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽" << END << std::endl;
    std::cout << GRE << "-----------RECIEVED-----------" << END << std::endl;
    std::cout << GRE << "socket  : " << END << sockfd << std::endl;
    // std::cout << GRE << "msg len : " << END << buf.length() << std::endl;
    std::cout << GRE << "msg     : " << END << buf << std::endl;
    std::cout << GRE << "△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△" << END << std::endl;
    buf.erase(buf.end() - _delimeter.length(), buf.end());
    return (bytes == -1 ? false : true);
}

bool	IRCServer::_send( int sockfd, const std::string &buf ) const
{
    std::string buf_delim(buf);
    int         total = 0;
    int         bytesLeft;
    int     	bytes;

    if (buf_delim.find(_delimeter) != buf_delim.length() - _delimeter.length())
        buf_delim += _delimeter;
    bytesLeft = buf_delim.length();
    while (bytesLeft > 0)
    {
        bytes = send(sockfd, buf_delim.c_str() + total, bytesLeft, 0);
        if (bytes < 0)
        {
            std::cerr << RED << strerror(errno) << END;
            break ;
        }
        total += bytes;
        bytesLeft -= bytes;
    }
    std::cout << YEL << "▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼" << END << std::endl;
    std::cout << YEL << "------------SENDED------------" << END << std::endl;
    std::cout << YEL << "socket  : " << END << sockfd << std::endl;
    // std::cout << YEL << "msg len : " << END << buf_delim.length() << std::endl;
    std::cout << YEL << "msg     : " << END << buf_delim << std::endl;
    std::cout << YEL << "▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲" << END << std::endl;
    return (bytes == -1 ? false : true);
}

void    IRCServer::_removeUser(int sockfd)
{
    std::multimap<std::string, User>::iterator  it;

    it = _users.begin();
    while (it != _users.end() && it->second.getSocket() != sockfd)
        it++;
    if (it != _users.end())
        _users.erase(it);
}

void    IRCServer::_addUser(int sockfd)
{
    User new_user;

    new_user.setSocket(sockfd);
    std::pair<std::string, User> tmp(std::string(""), new_user);
    _users.insert(tmp);
}

void    IRCServer::_addUser(const User &user)
{
    std::pair<std::string, User> tmp(user.getNickname(), user);
    _users.insert(tmp);
}

void IRCServer::_PRIVMSG(const Message &msg, const User &usr) {
	std::multimap<std::string, User>::iterator us_it;
    std::map<std::string, Channel>::iterator ch_it;

	us_it = this->_users.begin();
    ch_it = this->_channels.begin();
    
    for (int i = 0; i != msg.getParamets().size() - 1; ++i) {
	    us_it = this->_users.find(msg.getParamets()[i]);
        ch_it = this->_channels.find(msg.getParamets()[i]);
	    if (us_it != this->_users.end()) {
            std::string message(":" + msg.getPrefix() + " PRIVMSG " + us_it->second.getNickname() + " :" + msg.getParamets().back()); 
            std::cout << message << std::endl;
		    _send(us_it->second.getSocket(), message);
        }
        else if (ch_it != this->_channels.end()) {
            std::string message(":" + msg.getPrefix() + " PRIVMSG " + ch_it->second.getName() + " :" + msg.getParamets().back()); 
            std::cout << message << std::endl;
		    _send(us_it->second.getSocket(), message);
        }
	}
}

void    IRCServer::_CAP( const Message &msg, User &user )
{
    std::string	buf;

    if (msg.getCommand() != "CAP")
        return ;
    if (msg.getParamets().size() > 0 && msg.getParamets()[0] == "LS")
    {
        buf = "CAP * LS :";
        _send(user.getSocket(), buf);
    }
}

void    IRCServer::_PASS( const Message &msg, User &user )
{
    std::string	buf;

    if (msg.getCommand() != "PASS")
        return ;
    if (user.isPassworded())
    {
        buf = "462 :You may not reregister";
        _send(user.getSocket(), buf);
        return ;
    }
    if (msg.getParamets().size() == 0)
    {
        buf = "461 PASS :Not enough parameters";
        _send(user.getSocket(), buf);
        return ;
    }
    if (msg.getParamets()[0] == _password)
        user.switchPassword();
    else
    {
        if (user.getNickname().empty())
            buf = "464 * :Password incorrect";
        else
            buf = "464 " + user.getNickname() + " :Password incorrect";
        _send(user.getSocket(), buf);
    }
}

bool    IRCServer::_isCorrectNick( const std::string &nick )
{
    if (nick.length() > NICKLEN || nick.length() == 0)
        return (false);
    if (!(nick[0] >= 'a' && nick[0] <= 'z' || nick[0] >= 'A' && nick[0] <= 'Z'))
        return (false);
    for (size_t i = 1; i < nick.length(); ++i)
    {
        if (!(     nick[i] >= 'a' && nick[i] <= 'z'
                || nick[i] >= 'A' && nick[i] <= 'Z'
                || nick[i] >= '0' && nick[i] <= '9'
                || nick[i] == '-'
                || nick[i] == '['
                || nick[i] == ']'
                || nick[i] == '\\'
                || nick[i] == '\''
                || nick[i] == '^'
                || nick[i] == '{'
                || nick[i] == '}'
            ))
            return (false);
    }
    return (true);
}

void    IRCServer::_NICK( const Message &msg, User &user )
{
    std::string	buf;

    if (msg.getCommand() != "NICK")
        return ;
    if (msg.getParamets().size() == 0)
    {
        buf = "431 :No nickname given";
        _send(user.getSocket(), buf);
        return ;
    }
    if (!_isCorrectNick(msg.getParamets()[0]))
    {
        buf = "432 " + msg.getParamets()[0] + " :Erroneus nickname";
        _send(user.getSocket(), buf);
        return ;
    }
    if (_users.find(msg.getParamets()[0]) != _users.end())
    {
        buf = "433 " + msg.getParamets()[0] + " :Nickname is already in use";
        _send(user.getSocket(), buf);
        return ;
    }
    std::string oldNick(user.getNickname());
    User        copy(user);

    copy.setNickname(msg.getParamets()[0]);
    _removeUser(user.getSocket());
    _addUser(copy);
    if (oldNick.empty())
        buf = "NICK " + copy.getNickname();
    else
        buf = ":" + oldNick + " NICK :" + copy.getNickname();
    _send(user.getSocket(), buf);
}

void    IRCServer::_USER( const Message &msg, User &user )
{
    std::string	buf;

    if (msg.getCommand() != "USER")
        return ;
    if (msg.getParamets().size() < 4)
    {
        buf = "461 NICK :Not enough parameters";
        _send(user.getSocket(), buf);
        return ;
    }
    if (user.isLogged())
    {
        buf = "462 :Already registered";
        _send(user.getSocket(), buf);
        return ;
    }

    buf = "001 "
        + user.getNickname()
        + " :Welcome to the Internet Relay Network, "
        + user.getNickname()
        + "\r\n";
    buf += "002 "
        + user.getNickname()
        + " :Your host is <servername>, running version <version>"
        + "\r\n";
    buf += "003 "
        + user.getNickname()
        + " :This server was created <datetime>"
        + "\r\n";
    buf += "004 "
        + user.getNickname()
        + " <servername> 1.0/UTF-8 aboOirswx abcehiIklmnoOpqrstvz"
        + "\r\n";
    buf += "005 "
        + user.getNickname()
        + " PREFIX=(ohv)@\%+ CODEPAGES MODES=3 CHANTYPES=#&!+ MAXCHANNELS=20 NICKLEN=31 TOPICLEN=255 KICKLEN=255 NETWORK=school21 CHANMODES=beI,k,l,acimnpqrstz :are supported by this server"
        + "\r\n";
    _send(user.getSocket(), buf);
}

void    IRCServer::_PING( const Message &msg, User &user )
{
    std::string	buf;

    if (msg.getCommand() != "PING")
        return ;
    buf = "PONG " + _hostname;
    _send(user.getSocket(), buf);
}

void IRCServer::_NOTICE(const Message &msg, const User &usr) {
	// ne mozhet bit; gruppoj
	// is the same as PRIVMSG ? without sendng response to serv?
}

void IRCServer::_JOIN(const Message &msg, User &usr) {

	// JOIN <channel>{, <channel>}[<key>{, <key>}
	// 4.2.1 examples
	usr.setNickname("ebanat");
	Channel test_ch("klub");
	this->_channels.insert(std::make_pair("klub", test_ch));



	// # and &
	for (int i = 0; i < msg.getParamets().size(); i++) {

		std::map<std::string, Channel>::iterator ch_it;
		ch_it = this->_channels.find(msg.getParamets()[i]); // najti group

		if (ch_it != this->_channels.end()) {

			std::cout << "group exist!" << std::endl;
			// gruppa est' -> nado join
			
			// Если установлен пароль -> должен быть верным.
			// buff >> pass
			// if pass != get_group_pass -> invalid pass

			// check mode of channnel

			// check if he dosnt banned in channel

			// if user has no more than 10 channels
			// (405 ERR_TOOMANYCHANNELS)

			ch_it->second.addUser(usr);
		
			// if joined user -> send msg about new user to all
				// _PRIVMSG TO ALL USER IN GROUP

			// and Если JOIN прошла хорошо, пользователь получает топик канала
			ch_it->second.channel_info();	
		
		}
		else {

			std::cout << "group creating..." << std::endl;
			if (usr._nickname == "") // no name user
				return;

			Channel new_ch(msg.getParamets()[i]);
			this->_channels.insert(std::make_pair(new_ch._name, new_ch));
			new_ch.addUser(usr);
			new_ch.addChop(usr);

			// new_ch.channel_info();
			// info to server that usr is chop now to server

			std::string to_send = "now you an admin of " + new_ch._name + " group";
			std::cout << "!!!1111" << std::endl;
			this->_send(usr.getSocket(), to_send);
		}
	}
}

void IRCServer::_PART(const Message &msg, const User &usr) {
	// dolzhno rabotat' s neskol'kimi params(groups)!!
	// Параметры: <channel>{,<channel>}
	// Сообщение PART удаляет клиента, пославшего эту команду из списка
	// активных пользователей для всех каналов, указанных в параметре.
	// PART #twilight_zone // # is ok?

	// loop po vsem channels -> delete user//

	std::map<std::string, Channel>::iterator ch_it;
	ch_it = this->_channels.find(msg.getParamets()[0]); // najti group
	if (ch_it != this->_channels.end()) {
		// gruppa finded
		ch_it->second.removeUser(usr._nickname); // add getter getNick
		// msg to all sockets that user left channel
	}
	else {
		// group not finded -> return ERR
	}
}

void IRCServer::_OPER(const Message &msg) {
	// Команда: OPER
	// Параметры: <user> <password>

	// msg.getParamets()[0] - eto user?
	// msg.getParamets()[1] - eto pass?
	// pass == with server pass?

	std::string str_user = msg.getParamets()[0];
	std::multimap<std::string, User>::iterator us_it;

	us_it = this->_users.find(str_user); // najti group
	
	if (_users.count(str_user) > 1)
		std::cout << "more one user with this name" << std::endl;
		// ERR

	if (us_it != this->_users.end())
		this->_operators.insert(std::make_pair(str_user, &us_it->second));
	// notice server that str_name is serv operator now ..

}

void IRCServer::_LIST(const Message &msg) {
	// Команда: LIST
	// Параметры: [<channel>{,<channel>} [<server>]]
	// NO cout channel if it secret and user doesnt have the accses

	// if params > 1 - cout 1 channel + topic
	// CHECK IF MODE IS OK

	if (this->_channels.size()) {
		std::cout << "list of channels and their topics:" << std::endl;
		std::map<std::string, Channel>::iterator ch_it;
		ch_it = this->_channels.begin();
		std::cout << ch_it->first << " - " << ch_it->second._topic << std::endl; // GET TOPIC FUNC NEED
	}
}

void IRCServer::_NAMES(const Message &msg) {
	// Команда: NAMES
	// Параметры: [<channel>{,<channel>}]
	// esli NAMES -> to vse channels -> users
	// elsi names + chanlel -> vse users in channel
	// Имена каналов, которые они могут видеть это те, которые не приватные
	// (+p) или секретные (+s) // CHECK MODE

	// all channels
	if (this->_channels.size()) {
		std::map<std::string, Channel>::iterator ch_it;
		ch_it = this->_channels.begin();
		
		// std::map<std::string, User*>::iterator us_it; // for map of users in channel
		// for (; ch_it != this->_channels.end(); ch_it++)
			// ch_it->second._users; // make method show only users in this channel
		
	}
}

void IRCServer::_KICK(const Message &msg, const User &usr) {
	// Команда: KICK
	// Параметры: <channel> <user> [<comment>]

	std::string str_channel = msg.getParamets()[0]; // true??
	std::string str_user = msg.getParamets()[1]; // true??

	// channel exists?
	std::map<std::string, Channel>::iterator ch_it;
	ch_it = this->_channels.begin();
	// if (ch_it != this->_channels.end())
		//
	// if (ch_it->second._users.find(str_user))
		// ch_it->second.removeUser(str_user);
		// will socket of user deleted?
	

	// else
		// no channel
}

void IRCServer::_INVITE(const Message &msg) { // add USER

	// Команда: INVITE
	// Параметры: <nickname> <channel>

	std::string str_channel = msg.getParamets()[0]; // true??
	std::string str_user = msg.getParamets()[1]; // true??

	// 1. check if client who invites new user -> is choop in channel
	std::map<std::string, Channel>::iterator ch_it;
	ch_it =  this->_channels.find(str_channel);
	// if (ch_it != this->_channels.end())
		// ch_it->second.addChop(usr);


}
