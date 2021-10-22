#include "Client.hpp"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

Client::Client(const char *hostname, const char *port, const char *nick)
    : _hostname(hostname),
      _port(port),
      _nick(nick),
      _delimeter("\r\n")
{
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(_hostname, _port, &hints, &servinfo)) != 0)
    {
        std::cerr << RED << "getaddrinfo: " << stderr << END << std::endl;
        throw std::exception();
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            std::cerr << RED << "client: socket" << END << std::endl;
            continue;
        }

        if (connect(_sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(_sockfd);
            std::cerr << RED << "client: connect" << END << std::endl;
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        std::cerr << RED << "client: failed to connectn" << END << std::endl;
        throw std::exception();
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    std::cerr << RED << "client: connecting to " << s << END << std::endl;

    freeaddrinfo(servinfo);
}

void Client::run()
{
    std::string buf;

    buf = "PASS 1234" + std::string("\r\n") + "NICK " + std::string(_nick) + std::string("\r\n") + "USER myusernaaame * * *" + std::string("\r\n");
    _send(buf);
    int pid = fork();
    if (pid < 0)
        return;
    if (pid > 0)
    {
        while (21)
        {
            try
            {
                _recv(buf);
            }
            catch (const std::exception &e)
            {
                close(_sockfd);
                return;
            }
        }
    }
    else
    {
        while (21)
        {
            std::cout << BLU << "➡️  " << END;
            buf.clear();
            std::getline(std::cin, buf);
            _send(buf);
        }
    }
}

bool Client::_recv(std::string &buf) const
{
    char c_buf[BUF_SIZE];
    int bytesLeft;
    int bytes = 1;

    buf.clear();
    while (buf.find(_delimeter) == std::string::npos && buf.length() < sizeof(c_buf))
    {
        memset(c_buf, 0, sizeof(c_buf));
        bytes = recv(_sockfd, c_buf, sizeof(c_buf), MSG_PEEK);
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
            bytes = recv(_sockfd, c_buf, bytesLeft, 0);
            if (bytes < 0)
            {
                std::cerr << RED << strerror(errno) << END;
                throw std::exception();
            }
            if (bytes == 0)
                throw std::exception();
            bytesLeft -= bytes;
            buf += c_buf;
        }
    }
    buf.erase(buf.end() - _delimeter.length(), buf.end());
    std::cout << GRE << (char)8 << (char)8 << (char)8
              << (char)8 << (char)8 << (char)8
              << "💌 \"" << buf << "\"" << END << std::endl;
    std::cerr << BLU << "➡️  " << END;
    // std::cerr << "===" << buf.find(std::string("PING")) << std::endl;
    if (buf.find("PING") != -1)
        _send(std::string(":") + _nick + std::string(" PONG ") + _nick);
    return (bytes == -1 ? false : true);
}

bool Client::_send(const std::string &buf) const
{
    std::string buf_delim(buf);
    int total = 0;
    int bytesLeft;
    int bytes;

    if (buf_delim.find(_delimeter) != buf_delim.length() - _delimeter.length())
        buf_delim += _delimeter;
    bytesLeft = buf_delim.length();
    while (bytesLeft > 0)
    {
        bytes = send(_sockfd, buf_delim.c_str() + total, bytesLeft, 0);
        if (bytes < 0)
        {
            std::cerr << RED << strerror(errno) << END;
            break;
        }
        total += bytes;
        bytesLeft -= bytes;
    }
    return (bytes == -1 ? false : true);
}
