#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

#define RED "\033[31m"
#define GRE "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define END "\033[37m"

#define BUF_SIZE 512

class Client
{
private:
    int             _sockfd;
    const char*     _hostname;
    const char*     _port;
    const char*     _nick;
    struct addrinfo hints, *servinfo, *p;
    char            s[INET6_ADDRSTRLEN];
    std::string     _delimeter;

public:
    Client( const char* hostname, const char* port, const char* nick );
    void    run  ();
private:
    bool    _recv(       std::string &buf ) const;
    bool    _send( const std::string &buf ) const;
};

#endif /* CLIENT_HPP */