#include "IRCServer.hpp"
#include <string>

IRCServer::IRCServer(unsigned int port, std::string pass) :
	_port(port),
	_password(pass),
    _delimeter("\r\n")
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

    std::cout << GRE << "Server is started..." << END << std::endl;
	while (select(_max_fd + 1, &select_fds, NULL, NULL, NULL) != -1)
    {
		for (int i = 3; i < _max_fd + 1; i++)
        {
			if (!FD_ISSET(i, &select_fds) || i == _listener)
                continue ;
            std::string buf;

            // receiving data
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
            // command execution
            _execute(i, buf);
            
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

    if (buf.empty())
        return (false);
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

void    IRCServer::_removeUser( const std::string &nick )
{
    std::multimap<std::string, Channel>::iterator chit;
    std::multimap<std::string, User>   ::iterator uit;

    // removing from _users
    uit = _users.find(nick);
    if (uit != _users.end())
        _users.erase(uit);

    // removing from _channels
    for (chit = _channels.begin(); chit != _channels.end(); ++chit)
        chit->second.removeUser(nick);
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

void    IRCServer::_execute( int sockfd, const std::string &buf )
{
    std::multimap<std::string, User>::iterator  it;
            
    it = _users.begin();
    while (it != _users.end() && it->second.getSocket() != sockfd)
        it++;
    if (it == _users.end())
        return ; // no such user
    
    Message msg(buf, it->second);
    User    *user = &it->second;

    if (user->isPassworded() && user->isLogged())
    {
        _QUIT(msg, &user);
        if (user == NULL)
            return ;
        _KILL(msg, &user);
        if (user == NULL)
            return ;
    }

    _CAP (msg, *user);
    _PASS(msg, *user);
    _PING(msg, *user);
    if (user->isPassworded())
    {
        _NICK(msg, &user);
        _USER(msg, *user);
    }
    if (user->isPassworded() && user->isLogged())
    {
        _PRIVMSG(msg, *user);
        _JOIN   (msg, *user);
        _LIST   (msg, *user);
        _OPER   (msg, *user);
        _KICK   (msg, *user);
		if (utils::toUpper(msg.getCommand()) == "NAMES")
        	_NAMES  (msg, *user);
    }
}

void IRCServer::_PRIVMSG(const Message &msg, const User &usr) {
	std::multimap<std::string, User>::iterator us_it;
	std::map<std::string, Channel>::iterator ch_it;
	std::string buf;

    if (utils::toUpper(msg.getCommand()) != "PRIVMSG")
        return ;

    if (msg.getParamets().empty()) {
        buf = "411 :No recipient given PRIVMSG";
        _send(usr.getSocket(), buf);
        return ; 
    }

    if(msg.getParamets().size() == 1) {
        buf = "412 :No text to send";
        _send(usr.getSocket(), buf);
        return ; 
    }

    for (int i = 0; i != msg.getParamets().size() - 1; ++i) {
        for (int j = 0; j != msg.getParamets().size() - 1; ++j) {
            if (i != j && msg.getParamets()[i] == msg.getParamets()[j]) {
                buf = "407: " +  msg.getParamets()[i] + " :Duplicate recipients. No message delivered";
                us_it = this->_users.find(msg.getParamets()[i]);
                _send(us_it->second.getSocket(), buf);
                return ;
            }
        }
    }

	us_it = this->_users.begin();
    ch_it = this->_channels.begin();
    for (int i = 0; i != msg.getParamets().size() - 1; ++i) {
	    us_it = this->_users.find(msg.getParamets()[i]);
        ch_it = this->_channels.find(msg.getParamets()[i]);
	    if (us_it != this->_users.end()) {
            std::string message(":" + msg.getPrefix() 
                                    + " PRIVMSG " 
                                    + us_it->second.getNickname() 
                                    + " :" 
                                    + msg.getParamets().back()); 

		    _send(us_it->second.getSocket(), message);
        }
        else if (ch_it != this->_channels.end()) {
            std::string message(":" + msg.getPrefix() 
                                    + " PRIVMSG " 
                                    + ch_it->second.getName() 
                                    + " :" 
                                    + msg.getParamets().back()); 
            std::map<std::string, User*>::const_iterator us_ch_it;

            us_ch_it = ch_it->second.getUsers().begin();
            for (;us_ch_it != ch_it->second.getUsers().end(); ++us_ch_it) {
		        _send(us_ch_it->second->getSocket(), message);
            }
        }
        else{
            buf = "401: " + msg.getParamets()[i] + " :No such nick/channel";
            _send(usr.getSocket(), buf);
            return ; 
        }
	}
}

void    IRCServer::_CAP( const Message &msg, const User &user ) const
{
    std::string	buf;

    if (utils::toUpper(msg.getCommand()) != "CAP")
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

    if (utils::toUpper(msg.getCommand()) != "PASS")
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

void    IRCServer::_sendToChannel( const std::string &channel,
                        const std::string &buf,
                        const std::string &nick /* = "" */ )
{
    std::map<std::string, Channel>::const_iterator  chit;
    std::map<std::string, User*>::const_iterator    it;

    chit = _channels.find(channel);
    if (chit == _channels.end())
    {
        std::cerr << RED << "Something went wrong : _sendToChannel : No such channel"
                  << END << std::endl;
        return ;
    }
    it = chit->second.getUsers().begin();
    for (; it != chit->second.getUsers().end(); ++it)
    {
        if (it->first != nick)
            _send(it->second->getSocket(), buf);
    }
}

bool    IRCServer::_isCorrectNick( const std::string &nick )
{
    if (nick.length() > NICKLEN || nick.length() == 0)
        return (false);
    if (!((nick[0] >= 'a' && nick[0] <= 'z') || (nick[0] >= 'A' && nick[0] <= 'Z')))
        return (false);
    for (size_t i = 1; i < nick.length(); ++i)
    {
        if (!(     (nick[i] >= 'a' && nick[i] <= 'z')
                || (nick[i] >= 'A' && nick[i] <= 'Z')
                || (nick[i] >= '0' && nick[i] <= '9')
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

void    IRCServer::_NICK( const Message &msg, User **user )
{
    std::string	buf;

    if (utils::toUpper(msg.getCommand()) != "NICK")
        return ;
    if (msg.getParamets().size() == 0)
    {
        buf = "431 :No nickname given";
        _send((*user)->getSocket(), buf);
        return ;
    }
    if (!_isCorrectNick(msg.getParamets()[0]))
    {
        buf = "432 " + msg.getParamets()[0] + " :Erroneus nickname";
        _send((*user)->getSocket(), buf);
        return ;
    }
    if (_users.find(msg.getParamets()[0]) != _users.end())
    {
        buf = "433 " + msg.getParamets()[0] + " :Nickname is already in use";
        _send((*user)->getSocket(), buf);
        return ;
    }
    std::string oldNick((*user)->getNickname());
    User        copy(**user);

    copy.setNickname(msg.getParamets()[0]);
    _removeUser((*user)->getSocket());
    _addUser(copy);
    if (oldNick.empty())
        buf = "NICK " + copy.getNickname();
    else
        buf = ":" + oldNick + " NICK :" + copy.getNickname();
    (*user) = &(_users.find(copy.getNickname())->second);
    _send((*user)->getSocket(), buf);
}

void    IRCServer::_USER( const Message &msg, User &user )
{
    std::string	buf;

    if (utils::toUpper(msg.getCommand()) != "USER")
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
    user.switchLogged();
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

void    IRCServer::_PING( const Message &msg, const User &user ) const
{
    std::string	buf;

    if (utils::toUpper(msg.getCommand()) != "PING")
        return ;
    buf = "PONG " + _hostname;
    _send(user.getSocket(), buf);
}

void IRCServer::_NOTICE(const Message &msg, const User &usr) {
	std::multimap<std::string, User>::iterator us_it;
    std::map<std::string, Channel>::iterator ch_it;
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "NOTICE")
        return ;

    if (msg.getParamets().empty()) {
        return ; 
    }

    if(msg.getParamets().size() == 1) {
        return ; 
    }

    for (int i = 0; i != msg.getParamets().size() - 1; ++i) {
        for (int j = 0; j != msg.getParamets().size() - 1; ++j) {
            if (i != j && msg.getParamets()[i] == msg.getParamets()[j]) 
                return ;
        }
    }

	us_it = this->_users.begin();
    ch_it = this->_channels.begin();
    for (int i = 0; i != msg.getParamets().size() - 1; ++i) {
	    us_it = this->_users.find(msg.getParamets()[i]);
        ch_it = this->_channels.find(msg.getParamets()[i]);
	    if (us_it != this->_users.end()) {
            std::string message(":" + msg.getPrefix() 
                                    + " PRIVMSG " 
                                    + us_it->second.getNickname() 
                                    + " :" 
                                    + msg.getParamets().back()); 

		    _send(us_it->second.getSocket(), message);
        }
        else if (ch_it != this->_channels.end()) {
            std::string message(":" + msg.getPrefix() 
                                    + " PRIVMSG " 
                                    + ch_it->second.getName() 
                                    + " :" 
                                    + msg.getParamets().back()); 
            std::map<std::string, User*>::const_iterator us_ch_it;

            us_ch_it = ch_it->second.getUsers().begin();
            for (;us_ch_it != ch_it->second.getUsers().end(); ++us_ch_it) {
		        _send(us_ch_it->second->getSocket(), message);
            }
        }
        else
            return ; 
	}
}

void IRCServer::_JOIN(const Message &msg, User &usr) {

    if (utils::toUpper(msg.getCommand()) != "JOIN")
        return;

	std::string to_send;

    if (msg.getParamets().empty()) {
		to_send = "461 " + usr.getNickname() + " JOIN " + ":Not enough parameters";
		std::cout << usr.getNickname() + " JOIN " + ":Not enough parameters" << std::endl;
		_send(usr.getSocket(), to_send);
		return;
    }

	if (msg.getParamets()[0][0] != '#' && msg.getParamets()[0][0] != '&') {
		// first param should be group

		to_send = "400 " + usr.getNickname() + " JOIN " + ":Could not process invalid parameters";
		std::cout << usr.getNickname() + " JOIN " + ":Could not process invalid parameters" << std::endl;
		_send(usr.getSocket(), to_send);
		return;
	}

	std::vector<std::string> params;
	std::vector<std::string> passwords;
	for (int i = 0; i < msg.getParamets().size(); i++) {
		
        // check valid name of a group
		std::string tmp_param = msg.getParamets()[i];
		for (int k = 0; k < tmp_param.size(); k++) {
			if (tmp_param[k] == ' ' ||
				tmp_param[k] == ',' ||
				tmp_param[k] == '\a' ||
				tmp_param[k] == '\0' ||
				tmp_param[k] == '\r' ||
				tmp_param[k] == '\n') {
					
					to_send = "400 " + usr.getNickname() + " JOIN " + ":Could not process invalid parameters";
					std::cout << usr.getNickname() + " JOIN " + ":Could not process invalid parameters" << std::endl;
					_send(usr.getSocket(), to_send);
					return ;
			}
		}
		
		if (tmp_param[0] == '#' || tmp_param[0] == '&') {
			// tmp_param.erase(0, 1);
			params.push_back(tmp_param);
		}
		else {
			passwords.push_back(tmp_param);
		}
	}

	if (passwords.size() > params.size()) {
		std::cout << "more passes were provided than groups" << std::endl; // maybe set invalid params
		return;
	}

	for (int i = 0; i < params.size(); i++) {

		std::string tmp_group = params[i];
		std::map<std::string, Channel>::iterator ch_it;
		ch_it = this->_channels.find(tmp_group);

		if (ch_it != this->_channels.end()) {      
        
			try {
				passwords.at(i);
				// invalid pass
				if (!(ch_it->second._pass == passwords[i])) {

					to_send = "475 " + usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)";
					std::cout << usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)" << std::endl;
					_send(usr.getSocket(), to_send);
					return;
				}
			}
			catch( ... ) {
				if (!ch_it->second._pass.empty()) {
					to_send = "475 " + usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)";
					std::cout << usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)" << std::endl;
					_send(usr.getSocket(), to_send);
					return;
				}
			}
			std::cout << "whats inside" << std::endl; // to del

            std::map<std::string, User*>::const_iterator user_search_it;
			user_search_it = ch_it->second.getUsers().find(usr.getNickname());
			if (user_search_it != ch_it->second.getUsers().end()) {
				// user already in this group;
				return;
			}


			// check mode of channnel MODE

			// TEST Ban list

			// checking if user doesnt banned on channel
			for (int i = 0; i < ch_it->second._ban_list.size(); i++) {
				if (ch_it->second._ban_list[i] == usr.getNickname()) {
					to_send = "474 " + usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+b)";
					std::cout << usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+b)" << std::endl;
					_send(usr.getSocket(), to_send);
					return;
				}
			}

			// if more than 10 groups
			int num_groups_user_in = 0;
			std::map<std::string, Channel>::iterator max_group_it;
			max_group_it = this->_channels.begin();
			for (; max_group_it != this->_channels.end(); max_group_it++) {
			if (max_group_it->second.getUsers().find(usr.getNickname()) != max_group_it->second.getUsers().end())
				num_groups_user_in++;
			}
			if (num_groups_user_in > 10) {
				to_send = "405 " + usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels";
				std::cout << usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels" << std::endl;
				_send(usr.getSocket(), to_send);
				return;
			}

			_send(usr.getSocket(), "JOIN");
			ch_it->second.addUser(usr);
		
	 		// if joined user -> send msg about new user to all ?? need ??

			std::string to_send = "welcome to " + ch_it->second.getName() + " group\n";
			to_send += "the topic of the channel is " + ch_it->second.getTopic(); 
			this->_send(usr.getSocket(), to_send);
			_NAMES(msg, usr);
		}
		else {
			// if more than 10 groups
			int num_groups_user_in = 0;
			std::map<std::string, Channel>::iterator ban_it;
			ban_it = this->_channels.begin();
			for (; ban_it != this->_channels.end(); ban_it++) {
			if (ban_it->second.getUsers().find(usr.getNickname()) != ban_it->second.getUsers().end())
				num_groups_user_in++;
			}
			if (num_groups_user_in > 10) {
				to_send = "405 " + usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels";
				std::cout << usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels" << std::endl;
				_send(usr.getSocket(), to_send);
				return;
			}

			// if no nickname
			if (usr._nickname == "")
				return;

			Channel new_ch(tmp_group);
			new_ch.addUser(usr);
			new_ch.addChop(usr);
			
			try {
				passwords.at(i);
				new_ch._pass = passwords[i];
			}
			catch ( ... ) {}

			_send(usr.getSocket(), "JOIN");
			to_send = "now you an admin of " + new_ch.getName() + " group";
			this->_send(usr.getSocket(), to_send);

			this->_channels.insert(std::make_pair(new_ch.getName(), new_ch));
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
    if (utils::toUpper(msg.getCommand()) != "PART")
        return ;

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

void    IRCServer::_OPER( const Message &msg, const User &user )
{
    std::string                                 buf;
    std::multimap<std::string, User>::iterator  it;

    if (utils::toUpper(msg.getCommand()) != "OPER")
        return ;
    if (msg.getParamets().size() < 2)
        buf = ":" + _hostname + " 461 " + user.getNickname() + " OPER :Not enough parameters";
    else if (msg.getParamets()[0] != user.getNickname())
        buf = ":" + _hostname + " 491 " + user.getNickname() + " :No O-lines for your host";
    else if (msg.getParamets()[1] != _password)
        buf = ":" + _hostname + " 464 " + user.getNickname() + " :Password incorrect";
    else
    {
        it = _users.find(user.getNickname());
        if (it != _users.end())
        {
            _operators.insert(std::make_pair(user.getNickname(), &it->second));
            buf = ":" + _hostname + " 381 " + user.getNickname() + " :You are now an IRC operator";
        }
        else
            std::cerr << RED << "Something went wrong : OPER : No such user" << END << std::endl;
    }
    _send(user.getSocket(), buf);
}

void IRCServer::_LIST( const Message &msg, const User &user ) const
{
    std::map<std::string, Channel>::const_iterator  it;
    std::string                                     buf;

    if (utils::toUpper(msg.getCommand()) != "LIST")
        return ;
    buf = ":" + _hostname + " 321 " + user.getNickname() + " Channel :Users  Name";
    _send(user.getSocket(), buf);
    if (msg.getParamets().size() == 0)
    {
        for (it = _channels.begin(); it != _channels.end(); ++it)
        {
            const Channel       &channel = it->second;
            std::stringstream   ss;

            ss << channel.getUsers().size();
            buf = ":"
                + _hostname
                + " 322 "
                + user.getNickname()    + " "
                + channel.getName()     + " "
                + ss.str()              + " :"
                + channel.getTopic();
            _send(user.getSocket(), buf);
        }
    }
    else
    {
        for (size_t i = 0; i < msg.getParamets().size(); ++i)
        {
            it = _channels.find(msg.getParamets()[i]);
            
            if (it == _channels.end())
                continue ;

            const Channel       &channel = it->second;
            std::stringstream   ss;

            ss << channel.getUsers().size();
            buf = ":"
                + _hostname
                + " 322 "
                + user.getNickname()    + " "
                + channel.getName()     + " "
                + ss.str()              + " :"
                + channel.getTopic();
            _send(user.getSocket(), buf);
        }
    }
    buf = ":" + _hostname + " 323 " + user.getNickname() + " :End of /LIST";
    _send(user.getSocket(), buf);
}

void IRCServer::_NAMES(const Message &msg, const User &user) {

    std::map<std::string, Channel>::iterator ch_it;
	std::string message;
	std::string buf;
    std::vector<std::string> buf_string;

    // if (utils::toUpper(msg.getCommand()) != "NAMES")
        // return ;
	
	if (_channels.empty())
		return ;

    for (int i = 0; i < msg.getParamets().size(); ++i) {
        buf_string.push_back(msg.getParamets()[i]);
        if (!buf_string.empty() && buf_string[i][0] == '#') {
            // buf_string[i].erase(0, 1);
        }
        else
            buf_string.pop_back();
    }

	ch_it = _channels.begin();
	if (! buf_string.empty()) {
		for (int i = 0; i <  buf_string.size(); ++i) {
			ch_it = this->_channels.find( buf_string[i]);
			if (ch_it != _channels.end()) {
				std::map<std::string, User*>::const_iterator ch_us_it; 
				std::map<std::string, User*>::const_iterator ch_chops_it;
				ch_us_it = ch_it->second.getUsers().begin();
				message = "353 " + user.getNickname() 
								 + " = "
								 + ch_it->second.getName() 
								 + " :";
				for (; ch_us_it != ch_it->second.getUsers().end(); ++ch_us_it) {
					ch_chops_it = ch_it->second.getChops().find(ch_us_it->second->getNickname());
					if (ch_chops_it != ch_it->second.getChops().end())
						buf += "@" + ch_us_it->second->getNickname() + " ";
					else
						buf += ch_us_it->second->getNickname() + " ";
				}
				_send(user.getSocket(), message + buf);
                message = "366 "  + user.getNickname() 
                                    + " "
                                    + ch_it->second.getName() 
                                    + " :End of /NAMES list";
                _send(user.getSocket(), message);
			}
		}
	}
}

void IRCServer::_INVITE(const Message &msg) { // add USER

	// Команда: INVITE
	// Параметры: <nickname> <channel>

	std::string str_channel = msg.getParamets()[0]; // true??
	std::string str_user = msg.getParamets()[1]; // true??

    if (utils::toUpper(msg.getCommand()) != "INVITE")
        return ;
	// 1. check if client who invites new user -> is choop in channel
	std::map<std::string, Channel>::iterator ch_it;
	ch_it =  this->_channels.find(str_channel);
	// if (ch_it != this->_channels.end())
		// ch_it->second.addChop(usr);


}

void IRCServer::_QUIT( const Message &msg, User **user )
{
    std::string                                     buf;
    std::map<std::string, Channel>::const_iterator  cit;
    std::map<std::string, User*>::  const_iterator  uit;

    if (utils::toUpper(msg.getCommand()) != "QUIT")
        return ;
    if (msg.getParamets().empty())
        buf = ":" + (*user)->getNickname() + " QUIT :Quit: " + (*user)->getNickname();
    else
        buf = ":" + (*user)->getNickname() + " QUIT :Quit: " + msg.getParamets()[0];
    for (cit = _channels.begin(); cit != _channels.end(); ++cit)
    {
        uit = cit->second.getUsers().find((*user)->getNickname());
        if (uit != cit->second.getUsers().end())
            _sendToChannel(cit->second.getName(), buf, (*user)->getNickname());
    }
    buf = "ERROR :Closing Link";
    _send((*user)->getSocket(), buf);

    // removing the user
    close((*user)->getSocket());
    FD_CLR((*user)->getSocket(), &this->_client_fds);
    _removeUser((*user)->getNickname());
    *user = NULL;
}

void IRCServer::_KILL( const Message &msg, User **user )
{
    std::string                                     buf;
    std::map<std::string, Channel>::const_iterator  cit;
    std::map<std::string, User*>::  const_iterator  uit;

    if (utils::toUpper(msg.getCommand()) != "KILL")
        return ;
    if (msg.getParamets().size() < 2)
    {
        buf = ":" + _hostname + " 461 KILL :Not enough parameters";
        _send((*user)->getSocket(), buf);
        return ;
    }
    if (_operators.find((*user)->getNickname()) == _operators.end())
    {
        buf = ":" + _hostname + " 481 " + (*user)->getNickname()
            + " :Permission Denied- You're not an IRC operator";
        _send((*user)->getSocket(), buf);
        return ;
    }
    if (_users.find(msg.getParamets()[0]) == _users.end())
    {
        buf = ":" + _hostname + " 401 " + msg.getParamets()[0]
            + " :No such nick";
        _send((*user)->getSocket(), buf);
        return ;
    }
    buf = ":" + (*user)->getNickname() + " KILL "
        + msg.getParamets()[0] + " :" + msg.getParamets()[1];
    _send((*user)->getSocket(), buf);
    buf = ":localhost QUIT :Killed (" + (*user)->getNickname()
        + " (" + msg.getParamets()[1] + "))";

    // sending quit message to all
    User *killedUser = &_users.find(msg.getParamets()[0])->second;
    User tmp;
    tmp.setNickname(_hostname);
    _QUIT(Message(buf, tmp), &killedUser);
}

void IRCServer::_KICK( const Message &msg, const User &user )
{
    std::string                                     buf;
    // std::map<std::string, Channel>::const_iterator  cit;
    // std::map<std::string, User*>::  const_iterator  uit;

    if (utils::toUpper(msg.getCommand()) != "KICK")
        return ;
    if (msg.getParamets().size() < 2)
    {
        buf = ":" + _hostname + " 461 KILL :Not enough parameters";
        _send(user.getSocket(), buf);
        return ;
    }
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        std::cerr << BLU << it->first << END << std::endl;
    }
    if (_channels.find(msg.getParamets()[0]) == _channels.end())
    {
        buf = ":" + _hostname + " 403 " + user.getNickname() + " "
            + msg.getParamets()[0] + " :No such channel";
        _send(user.getSocket(), buf);
        return ;
    }
    Channel &channel = _channels.find(msg.getParamets()[0])->second;
    if (channel.getUsers().find(user.getNickname()) == channel.getUsers().end())
    {
        buf = ":" + _hostname + " 442 "
            + channel.getName() + " :You're not on that channel";
        _send(user.getSocket(), buf);
        return ;
    }
    if (channel.getChops().find(user.getNickname()) == channel.getChops().end())
    {
        buf = ":" + _hostname + " 482 " + user.getNickname() + " "
            + channel.getName() + " :You're not channel operator";
        _send(user.getSocket(), buf);
        return ;
    }
    if (channel.getUsers().find(msg.getParamets()[1]) == channel.getUsers().end())
    {
        buf = ":" + _hostname + " 441 " + msg.getParamets()[1] + " "
            + channel.getName() + " :They aren't on that channel";
        _send(user.getSocket(), buf);
        return ;
    }
    buf = ":" + user.getNickname() + " KICK "
        + msg.getParamets()[0] + " " + msg.getParamets()[1];
    _send(channel.getUsers().find(msg.getParamets()[1])->second->getSocket(), buf);
    // removing user
    channel.removeUser(msg.getParamets()[1]);
    // removing channel
    if (channel.getUsers().size() == 0)
    {
        _channels.erase(channel.getName());
        return ;
    }
    _sendToChannel(msg.getParamets()[0], buf, msg.getParamets()[1]);
}
