#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <sys/select.h>

#include <map>
#include <string>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <exception>

#include "User.hpp"
#include "Channel.hpp"
#include "Message.hpp"

class User;
class Channel;
class Message;

class IRCServer
{
public:
    int                              _max_fd; // ne bolshe 10
    int                              _server_fd;
    unsigned int                     _port;
    std::string                      _password;
    fd_set                           _client_fds;
    std::map<std::string, User>      _users;
    std::map<std::string, User*>     _operators;
    std::map<std::string, Channel>   _channels;


    explicit IRCServer( unsigned int port, std::string pass );
    ~IRCServer();
    void        start();
    // void        stop ();

    private:
        std::string recv( User & user );
        void        send( Message & msg );

};

#endif
