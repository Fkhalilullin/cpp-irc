#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <map>
#include <string>
#include <csignal>
#include <vector>
#include <sstream>
#include <iostream>
#include <exception>
#include <algorithm>

#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "User.hpp"
#include "utils.hpp"
#include "Message.hpp"
#include "Channel.hpp"

#define RED "\033[31m"
#define GRE "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define END "\033[37m"

#define NICKLEN     9

class User;
class Channel;
class Message;

void    sigintCatcher(int sig);

class IRCServer
{
    private:
        int                              _max_fd;
        int                              _listener;
        unsigned int                     _port;
        fd_set                           _client_fds;
        struct sockaddr_in               _serverAdress;
        std::string                      _buffer;
        std::string                      _hostname;
        std::string                      _password;
        std::string                      _delimeter;
        std::multimap<std::string, User> _users;
        std::map<std::string, User*>     _operators;
        std::map<std::string, Channel>   _channels;

    public:
        explicit IRCServer( unsigned int port, std::string pass );
        ~IRCServer();
        void    start();

    private:
        void    _stop      ();
        void    _accept    ();
        bool    _recv      ( int sockfd,       std::string &buf );
        bool    _send      ( int sockfd, const std::string &buf );
        void    _exec      ( const Message &msg );
        void    _addUser   ( int sockfd         );
        void    _addUser   ( const User &user   );
        void    _removeUser( int sockfd         );
        void    _removeUser( const std::string &nick );
        bool    _isCorrectNick( const std::string &nick );
        void    _sendToJoinedChannels( const std::string &nick,
                                       const std::string &buf );

        void    _sendToChannel( const std::string &channel,
                                const std::string &buf,
                                const std::string &nick = "" );
        std::multimap<std::string, User>::iterator    _getUser( int sockfd );

		void    _execute( int sockfd, const std::string &buf );
        void    _PRIVMSG( const Message &msg, const User &usr);
        void    _CAP    ( const Message &msg, const User &user );
        void    _PASS   ( const Message &msg, User &user );
        void    _NICK   ( const Message &msg, User **user );
        void    _USER   ( const Message &msg, User &user );

		void    _PING   ( const Message &msg, const User &user );
        void    _OPER   ( const Message &msg, const User &user );
		void    _NOTICE (const Message &msg);
		void    _JOIN   (const Message &msg, User &usr);
		void    _PART   (const Message &msg, const User &usr);
		void    _OPER   (const Message &msg);
		void    _LIST   (const Message &msg, const User &user);
		void    _NAMES  (const Message &msg, const User &user);
        void    _QUIT   ( const Message &msg, User **user );
        void    _KILL   ( const Message &msg, User **user );
        void    _KICK   ( const Message &msg, const User &user );

        void    _TOPIC  (const Message &msg, const User &user);
        void    _INVITE (const Message &msg, const User &user);
};

#endif
