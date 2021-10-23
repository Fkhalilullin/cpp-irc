#include "Client.hpp"
#include "../server/includes/User.hpp"
#include "../server/includes/Message.hpp"
#include "../server/includes/utils.hpp"
#include <fstream>

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

Client::Client(const char *hostname, const char *port, const char* pass)
    : _hostname(hostname),
      _port(port),
      _nick("bot"),
      _delimeter("\r\n"),
      _pass(pass)
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

    buf = "PASS " + _pass + std::string("\r\n")

          + "USER myusernaaame * * *" + std::string("\r\n") + "NICK " + std::string(_nick) + std::string("\r\n");
    _send(buf);
    User usr;
    usr.setNickname("bot");
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

        std::string msg_str;
        std::string cmd_str;
        Message msg(buf, usr);
        std::ifstream fin;

        if (utils::toUpper(msg.getCommand()) == "INVITE")
        {
            msg_str = "JOIN ";
            if (msg.getParamets().size() == 2)
            {
                msg_str += msg.getParamets()[1];
                _send(msg_str);
            }
            else
                continue;
        }
        else if (utils::toUpper(msg.getCommand()) == "PRIVMSG")
        {
            cmd_str = "PRIVMSG ";
            if (!msg.getParamets().empty())
            {
                if (msg.getParamets().back().find("zapuskaem") != std::string::npos ||
                    msg.getParamets().back().find("gusya") != std::string::npos ||
                    msg.getParamets().back().find("rabotyagi") != std::string::npos ||
                    msg.getParamets().back().find("запускаем") != std::string::npos ||
                    msg.getParamets().back().find("гуся") != std::string::npos ||
                    msg.getParamets().back().find("работяги") != std::string::npos ||
                    msg.getParamets().back().find("Zapuskaem") != std::string::npos ||
                    msg.getParamets().back().find("Gusya") != std::string::npos ||
                    msg.getParamets().back().find("Rabotyagi") != std::string::npos ||
                    msg.getParamets().back().find("Запускаем") != std::string::npos ||
                    msg.getParamets().back().find("Гуся") != std::string::npos ||
                    msg.getParamets().back().find("Работяги") != std::string::npos)
                {
                    for (size_t i = 0; i < msg.getParamets().size() - 1; ++i)
                        cmd_str += msg.getParamets()[i] + " ";
                    cmd_str += ":";
                    fin.open("utils/gus.txt");
                    _printMsg(cmd_str, fin);
                }
                else if (msg.getParamets().back().find("Putin") != std::string::npos ||
                         msg.getParamets().back().find("putin") != std::string::npos ||
                         msg.getParamets().back().find("Путин") != std::string::npos ||
                         msg.getParamets().back().find("путин") != std::string::npos)
                {
                    for (size_t i = 0; i < msg.getParamets().size() - 1; ++i)
                        cmd_str += msg.getParamets()[i] + " ";
                    cmd_str += ":";
                    fin.open("utils/putin.txt");
                    _printMsg(cmd_str, fin);
                }
                else if (msg.getParamets().back().find("Shrek") != std::string::npos ||
                         msg.getParamets().back().find("shrek") != std::string::npos ||
                         msg.getParamets().back().find("Шрек") != std::string::npos  ||
                         msg.getParamets().back().find("шрек") != std::string::npos)
                {
                    for (size_t i = 0; i < msg.getParamets().size() - 1; ++i)
                        cmd_str += msg.getParamets()[i] + " ";
                    cmd_str += ":";
                    fin.open("utils/shrek.txt");
                    _printMsg(cmd_str, fin);
                }
            }
            else
                continue;
        }
    }
}

bool Client::_recv(std::string &buf)
{
    char    c_buf[512];
    size_t  bytesLeft;
    int     bytes = 1;
    bool    res;

    buf.clear();
    while (buf.find(_delimeter) == std::string::npos && _recvBuffer.size() + buf.size() < sizeof(c_buf))
    {
        memset(c_buf, 0, sizeof(c_buf));
        bytes = recv(_sockfd, c_buf, sizeof(c_buf) - 1 - (_recvBuffer.size() + buf.size()), MSG_PEEK);
        if (bytes < 0)
        {
            if (errno == EAGAIN)
            {
                _recvBuffer += buf;
                return (false);
            }
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
                if (errno == EAGAIN)
                {
                    _recvBuffer += buf;
                    return (false);
                }
                std::cerr << RED << strerror(errno) << END;
                throw std::exception();
            }
            if (bytes == 0)
                throw std::exception();
            bytesLeft -= bytes;
            buf += c_buf;
        }
    }
    if (buf.find(_delimeter) == std::string::npos)
        res = false;
    else
        res = true;
    _recvBuffer += buf;
    buf = _recvBuffer;
    _recvBuffer.clear();
    buf.erase(buf.end() - _delimeter.length(), buf.end());
    std::cout << GRE << "💌 \"" << buf << "\"" << END << std::endl;
    if (buf.find("PING") != std::string::npos)
        _send(std::string(":") + _nick + std::string(" PONG ") + _nick);
    return (res);
}

bool Client::_send(const std::string &buf)
{
    std::string buf_delim(buf);
    int total = 0;
    int bytesLeft;
    int bytes;

    std::cout << YEL << "▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼" << END << std::endl;
    std::cout << YEL << "------------SENDED------------" << END << std::endl;
    std::cout << YEL << "msg     : " << END << buf << std::endl;
    std::cout << YEL << "▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲" << END << std::endl;
    if (buf_delim.find(_delimeter) != buf_delim.length() - _delimeter.length())
        buf_delim += _delimeter;
    bytesLeft = buf_delim.length();
    while (bytesLeft > 0)
    {
        bytes = send(_sockfd, buf_delim.c_str() + total, bytesLeft, 0);
        if (bytes < 0)
        {
            if (errno == EAGAIN)
            {
                _sendBuffer = buf_delim.c_str() + total;
                return (false);
            }
            std::cerr << RED << strerror(errno) << END;
            break;
        }
        total += bytes;
        bytesLeft -= bytes;
    }
    return (bytes == -1 ? false : true);
}

void Client::_printMsg(const std::string &cmd_str, std::ifstream &fin)
{
    std::string str;
    std::string buf;

    if (!fin.is_open())
        return;
    while (1)
    {
        buf = cmd_str;
        std::getline(fin, str);
        buf += str;
        _send(buf);
        usleep(10000);
        buf.clear();
        if (fin.eof())
            break;
    }
    fin.close();
}
