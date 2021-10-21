#include "../includes/IRCServer.hpp"

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

    signal(SIGPIPE, SIG_IGN);
}

IRCServer::~IRCServer() {}

void IRCServer::_accept()
{
    struct sockaddr_in temp;

    socklen_t socklen = sizeof(struct sockaddr_in); // temp
    int new_fd = accept(_listener, (struct sockaddr*)&temp, &socklen); // reinpretet cast ? // eto new client socket
    if (new_fd == -1)
        throw std::invalid_argument(strerror(errno));
    fcntl(_listener, F_SETFL, O_NONBLOCK);
    _addUser(new_fd);
    if (_max_fd < new_fd)
        _max_fd = new_fd;
    FD_SET(new_fd, &_client_fds);
    std::cout << GRE << "New connetion on socket : " << new_fd << END << std::endl;
}

void IRCServer::start()
{
    std::multimap<std::string, User>::iterator  uit;

	_listener = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
	if (_listener < 0)
		throw std::invalid_argument(strerror(errno));
    _max_fd = _listener;

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
            uit = _getUser(i);

            // receiving data
            try
            {
                if ( _recv(i, buf) )
                {
                    uit->second.appendBuffer(buf);
                    buf = uit->second.getBuffer();
                    uit->second.clearBuffer();
                }
                else
                {
                    uit->second.appendBuffer(buf);
                    continue ;
                }
            }
            catch (const std::exception& e)
            {
                User* user = &(uit->second);
                _QUIT(Message(std::string("QUIT :Remote host closed the connection"), *user), &user);
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
    int     res;

    buf.clear();
    while (buf.find(_delimeter) == std::string::npos
                            && buf.length() < sizeof(c_buf))
    {
        memset(c_buf, 0, sizeof(c_buf));
        bytes = recv(sockfd, c_buf, sizeof(c_buf), MSG_PEEK);
        if (bytes < 0)
        {
            if (errno == EAGAIN)
                return (false);

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
    if (buf.find(_delimeter) == -1)
        res = false;
    else
        res = true;
    std::cout << GRE << "▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽" << END << std::endl;
    std::cout << GRE << "-----------RECIEVED-----------" << END << std::endl;
    std::cout << GRE << "socket  : " << END << sockfd << std::endl;
    // std::cout << GRE << "msg len : " << END << buf.length() << std::endl;
    std::cout << GRE << "msg     : " << END << buf << std::endl;
    std::cout << GRE << "△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△" << END << std::endl;
    buf.erase(buf.end() - _delimeter.length(), buf.end());
    return (res);
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

    // removing from _channels
    for (chit = _channels.begin(); chit != _channels.end(); ++chit)
        chit->second.removeUser(nick);
    // removing from _users
    uit = _users.find(nick);
    if (uit != _users.end())
        _users.erase(uit);

    // removing channel
    for (chit = _channels.begin(); chit != _channels.end(); ++chit) {
        if (chit->second.getUsers().empty()) {
            this->_channels.erase(chit);
            chit = _channels.begin();
            if (chit == this->_channels.end())
                return ;
        }
    }
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


void    IRCServer::_sendToJoinedChannels( const std::string &nick, const std::string &buf ) const
{
    std::vector<std::string>                        alreadySent;
    std::map<std::string, Channel>::const_iterator  chit;
    std::map<std::string, User*>  ::const_iterator  uit;
    std::vector<std::string>      ::const_iterator  vit;


    // passing through all channels
    for (chit = _channels.begin(); chit != _channels.end(); ++chit)
    {
        const Channel &channel = chit->second;

        // found a user by nick in the channel ?
        if (channel.getUsers().find(nick) != channel.getUsers().end())
        {
            // passing through all users from channel
            for (uit = channel.getUsers().begin(); uit != channel.getUsers().end(); ++uit)
            {
                const User &user = *uit->second;

                vit = std::find(alreadySent.begin(), alreadySent.end(), user.getNickname());
                if (vit == alreadySent.end() && user.getNickname() != nick)
                {
                    alreadySent.push_back(user.getNickname());
                    _send(user.getSocket(), buf);
                }
            }
        }
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

    if (_password.empty() && !user->isPassworded())
	    user->switchPassword();
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
        _PART   (msg, *user);
        _LIST   (msg, *user);
        _OPER   (msg, *user);
        _KICK   (msg, *user);
        _NAMES  (msg, *user);
        _TOPIC  (msg, *user);
        _INVITE (msg, *user);
    }
}

std::multimap<std::string, User>::iterator    IRCServer::_getUser( int sockfd )
{
    std::multimap<std::string, User>::iterator  it;

    it = _users.begin();
    while (it != _users.end() && it->second.getSocket() != sockfd)
        it++;
    return (it);
}

