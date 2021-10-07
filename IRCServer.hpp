#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <sys/select.h>

#include <map>
#include <string>
// #include <string.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <exception>
#include <sys/time.h>


#include "User.hpp"
#include "Channel.hpp"
#include "Message.hpp"

// clean it
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>	//close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO
#include <netdb.h> // proto
#include <iostream>
#include <sstream>
#include <vector>


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
    std::vector<User>                _unloggedUsers;
    std::map<std::string, User*>     _operators;
    std::map<std::string, Channel>   _channels;


    explicit IRCServer( unsigned int port, std::string pass );
    ~IRCServer();
    void        start();
    // void        stop ();

    private:
        std::string _recv( User & user );
        void        _send( Message & msg );
        void        _exec( Message &msg);

};

#endif
