#include "../includes/IRCServer.hpp"

void IRCServer::_PRIVMSG(const Message &msg, const User &usr)
{
    std::multimap<std::string, User>::iterator us_it;
    std::map<std::string, Channel>::iterator ch_it;
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "PRIVMSG")
        return;

    if (msg.getParamets().empty())
    {
        buf = ":" + this->_hostname + " 411 " + usr.getNickname() +
              +" :No recipient given PRIVMSG";
        _send(usr.getSocket(), buf);
        return;
    }

    if (msg.getParamets().size() == 1)
    {
        buf = ":" + this->_hostname + " 412 " + usr.getNickname() + " :No text to send";
        _send(usr.getSocket(), buf);
        return;
    }

    for (size_t i = 0; i != msg.getParamets().size() - 1; ++i)
    {
        for (size_t j = 0; j != msg.getParamets().size() - 1; ++j)
        {
            if (i != j && msg.getParamets()[i] == msg.getParamets()[j])
            {
                buf = ":" + this->_hostname + " 407 " + usr.getNickname() + " " + msg.getParamets()[i] + " :Duplicate recipients. No message delivered";
                us_it = this->_users.find(msg.getParamets()[i]);
                _send(us_it->second.getSocket(), buf);
                return;
            }
        }
    }

    us_it = this->_users.begin();
    ch_it = this->_channels.begin();
    for (size_t i = 0; i != msg.getParamets().size() - 1; ++i)
    {
        us_it = this->_users.find(msg.getParamets()[i]);
        ch_it = this->_channels.find(msg.getParamets()[i]);
        if (us_it != this->_users.end())
        {
            std::string message(":" + msg.getPrefix() + " PRIVMSG " + us_it->second.getNickname() + " :" + msg.getParamets().back());

            _send(us_it->second.getSocket(), message);
        }
        else if (ch_it != this->_channels.end())
        {
            std::string message(":" + msg.getPrefix() + " PRIVMSG " + ch_it->second.getName() + " :" + msg.getParamets().back());

            _sendToChannel(ch_it->second.getName(), message, msg.getPrefix());
        }
        else
        {
            buf = ":" + this->_hostname + " 401 " + usr.getNickname() + " " + msg.getParamets()[i] + " :No such nick/channel";
            _send(usr.getSocket(), buf);
            return;
        }
    }
}

void IRCServer::_CAP(const Message &msg, const User &user)
{
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "CAP")
        return;
    if (msg.getParamets().size() > 0 && msg.getParamets()[0] == "LS")
    {
        buf = "CAP * LS :";
        _send(user.getSocket(), buf);
    }
}

void IRCServer::_PASS(const Message &msg, User &user)
{
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "PASS")
        return;
    if (user.isPassworded())
    {
        buf = "462 :You may not reregister";
        _send(user.getSocket(), buf);
        return;
    }
    if (msg.getParamets().size() == 0)
    {
        buf = "461 PASS :Not enough parameters";
        _send(user.getSocket(), buf);
        return;
    }
    if (msg.getParamets()[0] == _password)
        user.unablePassword();
    else
    {
        if (user.getNickname().empty())
            buf = "464 * :Password incorrect";
        else
            buf = "464 " + user.getNickname() + " :Password incorrect";
        _send(user.getSocket(), buf);
    }
}

bool IRCServer::_isCorrectNick(const std::string &nick)
{
    if (nick.length() > NICKLEN || nick.length() == 0)
        return (false);
    if (!((nick[0] >= 'a' && nick[0] <= 'z') || (nick[0] >= 'A' && nick[0] <= 'Z')))
        return (false);
    for (size_t i = 1; i < nick.length(); ++i)
    {
        if (!((nick[i] >= 'a' && nick[i] <= 'z') || (nick[i] >= 'A' && nick[i] <= 'Z') || (nick[i] >= '0' && nick[i] <= '9') || nick[i] == '-' || nick[i] == '[' || nick[i] == ']' || nick[i] == '\\' || nick[i] == '\'' || nick[i] == '^' || nick[i] == '{' || nick[i] == '}'))
            return (false);
    }
    return (true);
}

void IRCServer::_NICK(const Message &msg, User **user)
{
    std::string buf;
    std::map<std::string, Channel>::iterator chit;

    if (utils::toUpper(msg.getCommand()) != "NICK")
        return;

    if (msg.getParamets().size() == 0)
    {
        buf = "431 :No nickname given";
        _send((*user)->getSocket(), buf);
        return;
    }
    if (!_isCorrectNick(msg.getParamets()[0]))
    {
        buf = "432 " + msg.getParamets()[0] + " :Erroneus nickname";
        _send((*user)->getSocket(), buf);
        return;
    }
    if (_users.find(msg.getParamets()[0]) != _users.end())
    {
        buf = "433 " + msg.getParamets()[0] + " :Nickname is already in use";
        _send((*user)->getSocket(), buf);
        return;
    }
    // main command processing
    std::string oldNick((*user)->getNickname());
    std::string newNick(msg.getParamets()[0]);
    User copy(**user);

    copy.setNickname(newNick);
    _removeUser((*user)->getSocket());
    _addUser(copy);
    User &newUser = _users.find(newNick)->second;

    if (oldNick != "")
    {
        // removing from channels
        for (chit = _channels.begin(); chit != _channels.end(); ++chit)
        {
            Channel &channel = chit->second;

            // is chop?
            if (channel.removeChop(oldNick))
                channel.addChop(newUser);
            // is user
            if (channel.removeUser(oldNick))
            {
                std::cout << "user added???" << std::endl;
                channel.addUser(newUser);
            }
        }
        // is server operator
        if (_operators.erase(oldNick))
            _operators.insert(std::make_pair(newNick, &newUser));
    }

    if (oldNick.empty())
        buf = "NICK " + newNick;
    else
        buf = ":" + oldNick + " NICK :" + newNick;
    _sendToJoinedChannels(newNick, buf);
    (*user) = &(_users.find(newNick)->second);
    _send((*user)->getSocket(), buf);
    (*user)->unableNick();
    if ((*user)->isNick() && (*user)->isUser() && !(*user)->isLogged())
    {
        (*user)->unableLogged();
        buf = "001 " + (*user)->getNickname() + " :Welcome to the Internet Relay Network, " + (*user)->getNickname() + "\r\n";
        buf += "002 " + (*user)->getNickname() + " :Your host is " + _hostname + ", running version <version>" + "\r\n";
        buf += "003 " + (*user)->getNickname() + " :This server was created <datetime>" + "\r\n";
        buf += "004 " + (*user)->getNickname() + " " + _hostname + " 1.0/UTF-8 aboOirswx abcehiIklmnoOpqrstvz" + "\r\n";
        buf += "005 " + (*user)->getNickname() + " PREFIX=(ohv)@\%+ CODEPAGES MODES=3 CHANTYPES=#&!+ MAXCHANNELS=20 \
                NICKLEN=31 TOPICLEN=255 KICKLEN=255 NETWORK=school21 \
                CHANMODES=beI,k,l,acimnpqrstz :are supported by this server";
        _send((*user)->getSocket(), buf);
    }
}

void IRCServer::_USER(const Message &msg, User &user)
{
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "USER")
        return;
    if (msg.getParamets().size() < 4)
    {
        buf = "461 NICK :Not enough parameters";
        _send(user.getSocket(), buf);
        return;
    }
    if (user.isUser())
    {
        buf = "462 :Already registered";
        _send(user.getSocket(), buf);
        return;
    }
    user.unableUser();
    if (user.isNick() && user.isUser() && !user.isLogged())
    {
        user.unableLogged();
        buf = "001 " + user.getNickname() + " :Welcome to the Internet Relay Network, " + user.getNickname() + "\r\n";
        buf += "002 " + user.getNickname() + " :Your host is " + _hostname + ", running version <version>" + "\r\n";
        buf += "003 " + user.getNickname() + " :This server was created <datetime>" + "\r\n";
        buf += "004 " + user.getNickname() + " " + _hostname + " 1.0/UTF-8 aboOirswx abcehiIklmnoOpqrstvz" + "\r\n";
        buf += "005 " + user.getNickname() + " PREFIX=(ohv)@\%+ CODEPAGES MODES=3 CHANTYPES=#&!+ MAXCHANNELS=20 \
                NICKLEN=31 TOPICLEN=255 KICKLEN=255 NETWORK=school21 \
                CHANMODES=beI,k,l,acimnpqrstz :are supported by this server";
        _send(user.getSocket(), buf);
    }
}

void IRCServer::_PING(const Message &msg, const User &user)
{
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "PING")
        return;
    buf = "PONG " + _hostname;
    _send(user.getSocket(), buf);
}

void IRCServer::_NOTICE(const Message &msg)
{
    std::multimap<std::string, User>::iterator us_it;
    std::map<std::string, Channel>::iterator ch_it;
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "NOTICE")
        return;
    if (msg.getParamets().empty())
    {
        return;
    }
    if (msg.getParamets().size() == 1)
    {
        return;
    }
    for (size_t i = 0; i != msg.getParamets().size() - 1; ++i)
    {
        for (size_t j = 0; j != msg.getParamets().size() - 1; ++j)
        {
            if (i != j && msg.getParamets()[i] == msg.getParamets()[j])
                return;
        }
    }
    us_it = this->_users.begin();
    ch_it = this->_channels.begin();
    for (size_t i = 0; i != msg.getParamets().size() - 1; ++i)
    {
        us_it = this->_users.find(msg.getParamets()[i]);
        ch_it = this->_channels.find(msg.getParamets()[i]);
        if (us_it != this->_users.end())
        {
            std::string message(":" + msg.getPrefix() + " PRIVMSG " + us_it->second.getNickname() + " :" + msg.getParamets().back());

            _send(us_it->second.getSocket(), message);
        }
        else if (ch_it != this->_channels.end())
        {
            std::string message(":" + msg.getPrefix() + " PRIVMSG " + ch_it->second.getName() + " :" + msg.getParamets().back());

			_sendToChannel(ch_it->first, message, msg.getPrefix());
        }
        else
            return;
    }
}

void IRCServer::_JOIN(const Message &msg, User &usr)
{

    if (utils::toUpper(msg.getCommand()) != "JOIN")
        return;

    std::string to_send;

    if (msg.getParamets().empty())
    {
        to_send = "461 " + usr.getNickname() + " JOIN " + ":Not enough parameters";
        std::cout << usr.getNickname() + " JOIN " + ":Not enough parameters" << std::endl;
        _send(usr.getSocket(), to_send);
        return;
    }

    if (msg.getParamets()[0][0] != '#' && msg.getParamets()[0][0] != '&')
    {
        // first param should be group
        to_send = "400 " + usr.getNickname() + " JOIN " + ":Could not process invalid parameters";
        std::cout << usr.getNickname() + " JOIN " + ":Could not process invalid parameters" << std::endl;
        _send(usr.getSocket(), to_send);
        return;
    }

    std::vector<std::string> params;
    std::vector<std::string> passwords;
    for (size_t i = 0; i < msg.getParamets().size(); i++)
    {

        // check valid name of a group
        std::string tmp_param = msg.getParamets()[i];
        for (size_t k = 0; k < tmp_param.size(); k++)
        {
            if (tmp_param[k] == ' ' ||
                tmp_param[k] == ',' ||
                tmp_param[k] == '\a' ||
                tmp_param[k] == '\0' ||
                tmp_param[k] == '\r' ||
                tmp_param[k] == '\n')
            {

                to_send = "400 " + usr.getNickname() + " JOIN " + ":Could not process invalid parameters";
                std::cout << usr.getNickname() + " JOIN " + ":Could not process invalid parameters" << std::endl;
                _send(usr.getSocket(), to_send);
                return;
            }
        }

        if (tmp_param[0] == '#' || tmp_param[0] == '&')
            params.push_back(tmp_param);
        else
            passwords.push_back(tmp_param);
    }

    if (passwords.size() > params.size())
    {
        std::cout << "more passes were provided than groups" << std::endl;
        return;
    }

    for (size_t i = 0; i < params.size(); i++)
    {

        std::string tmp_group = params[i];
        std::map<std::string, Channel>::iterator ch_it;
        ch_it = this->_channels.find(tmp_group);

        if (ch_it != this->_channels.end())
        {

            try
            {
                passwords.at(i);
                // invalid pass
                if (!(ch_it->second.getPass() == passwords[i]))
                {

                    to_send = "475 " + usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)";
                    std::cout << usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)" << std::endl;
                    _send(usr.getSocket(), to_send);
                    return;
                }
            }
            catch (...)
            {
                if (!ch_it->second.getPass().empty())
                {
                    to_send = "475 " + usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)";
                    std::cout << usr.getNickname() + " " + ch_it->first + " " + ":Cannot join channel (+k)" << std::endl;
                    _send(usr.getSocket(), to_send);
                    return;
                }
            }

            std::map<std::string, User *>::const_iterator user_search_it;
            user_search_it = ch_it->second.getUsers().find(usr.getNickname());
            if (user_search_it != ch_it->second.getUsers().end())
            {
                // user already in this group;
                return;
            }

            // if more than 10 groups
            int num_groups_user_in = 0;
            std::map<std::string, Channel>::iterator max_group_it;
            max_group_it = this->_channels.begin();
            for (; max_group_it != this->_channels.end(); max_group_it++)
            {
                if (max_group_it->second.getUsers().find(usr.getNickname()) != max_group_it->second.getUsers().end())
                    num_groups_user_in++;
            }
            if (num_groups_user_in > 10)
            {
                to_send = "405 " + usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels";
                std::cout << usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels" << std::endl;
                _send(usr.getSocket(), to_send);
                return;
            }

            ch_it->second.addUser(usr);

            to_send = ":" + usr.getNickname() + " JOIN :" + ch_it->second.getName();

            this->_sendToChannel(ch_it->second.getName(), to_send);
        }
        else
        {
            // if more than 10 groups
            int num_groups_user_in = 0;
            std::map<std::string, Channel>::iterator ban_it;
            ban_it = this->_channels.begin();
            for (; ban_it != this->_channels.end(); ban_it++)
            {
                if (ban_it->second.getUsers().find(usr.getNickname()) != ban_it->second.getUsers().end())
                    num_groups_user_in++;
            }
            if (num_groups_user_in > 10)
            {
                to_send = "405 " + usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels";
                std::cout << usr.getNickname() + " " + ch_it->first + " " + ":You have joined too many channels" << std::endl;
                _send(usr.getSocket(), to_send);
                return;
            }

            // if no nickname
            if (usr.getNickname() == "")
                return;

            Channel new_ch(tmp_group);
            new_ch.addUser(usr);
            new_ch.addChop(usr);

            try
            {
                passwords.at(i);
                new_ch.setPass(passwords[i]);
            }
            catch (...)
            {
            }

            this->_channels.insert(std::make_pair(new_ch.getName(), new_ch));

            to_send = ":" + usr.getNickname() + " JOIN :" + new_ch.getName();
            this->_send(usr.getSocket(), to_send);
        }
    }
    std::string namesMsg;
    namesMsg = "NAMES ";
    for (size_t i = 0; i < params.size(); i++)
        namesMsg += params[i] + ",";

    namesMsg.erase(namesMsg.size() - 1);
    this->_NAMES(Message(namesMsg, usr), usr);
}

void IRCServer::_PART(const Message &msg, const User &usr)
{

    if (utils::toUpper(msg.getCommand()) != "PART")
        return;

    std::string to_send;

    if (msg.getParamets().empty())
    {
        to_send = "461 " + usr.getNickname() + " PART " + ":Not enough parameters";
        std::cout << usr.getNickname() + " PART " + ":Not enough parameters" << std::endl;
        _send(usr.getSocket(), to_send);
        return;
    }

    std::vector<std::string> params;
    std::string tmp_param;

    for (size_t i = 0; i < msg.getParamets().size(); i++)
    {
        if (msg.getParamets()[i][0] != '#' && msg.getParamets()[i][0] != '&')
        {
            to_send = "400 " + usr.getNickname() + " PART " + ":Could not process invalid parameters";
            std::cout << usr.getNickname() + " PART " + ":Could not process invalid parameters" << std::endl;
            _send(usr.getSocket(), to_send);
        }
        else
        {
            tmp_param = msg.getParamets()[i];
            params.push_back(tmp_param);
        }
    }

    for (size_t i = 0; i < params.size(); i++)
    {

        std::map<std::string, Channel>::iterator ch_it;
        ch_it = this->_channels.find(params[i]);
        if (ch_it != this->_channels.end())
        { // channel exists

            std::map<std::string, User *>::const_iterator user_search_it;
            user_search_it = ch_it->second.getUsers().find(usr.getNickname());

            // user in group. should be deleted
            if (user_search_it != ch_it->second.getUsers().end())
            {

                to_send = ":" + usr.getNickname() + " PART :" + ch_it->first;

                this->_sendToChannel(ch_it->first, to_send);
                ch_it->second.removeUser(usr.getNickname());

                // if group is empty - del it
                if (ch_it->second.getUsers().empty())
                    this->_channels.erase(ch_it->first);
            }
            else
            { // no this user in group
                to_send = "442 " + usr.getNickname() + " " + params[i] + " " + ":You're not on that channel";
                std::cout << usr.getNickname() + " " + params[i] + " " + ":You're not on that channel" << std::endl;
                _send(usr.getSocket(), to_send);
            }
        }
        else
        { // channel dosnt exist
            to_send = "403 " + usr.getNickname() + " " + params[i] + " " + ":No such channel";
            std::cout << usr.getNickname() + " " + params[i] + " " + ":No such channel" << std::endl;
            _send(usr.getSocket(), to_send);
        }
    }
}

void IRCServer::_OPER(const Message &msg, const User &user)
{
    std::string buf;
    std::multimap<std::string, User>::iterator it;

    if (utils::toUpper(msg.getCommand()) != "OPER")
        return;
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

void IRCServer::_LIST(const Message &msg, const User &user)
{
    std::map<std::string, Channel>::const_iterator it;
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "LIST")
        return;
    buf = ":" + _hostname + " 321 " + user.getNickname() + " Channel :Users  Name";
    _send(user.getSocket(), buf);
    if (msg.getParamets().size() == 0)
    {
        for (it = _channels.begin(); it != _channels.end(); ++it)
        {
            const Channel &channel = it->second;
            std::stringstream ss;

            ss << channel.getUsers().size();
            buf = ":" + _hostname + " 322 " + user.getNickname() + " " + channel.getName() + " " + ss.str() + " :" + channel.getTopic();
            _send(user.getSocket(), buf);
        }
    }
    else
    {
        for (size_t i = 0; i < msg.getParamets().size(); ++i)
        {
            it = _channels.find(msg.getParamets()[i]);

            if (it == _channels.end())
                continue;

            const Channel &channel = it->second;
            std::stringstream ss;

            ss << channel.getUsers().size();
            buf = ":" + _hostname + " 322 " + user.getNickname() + " " + channel.getName() + " " + ss.str() + " :" + channel.getTopic();
            _send(user.getSocket(), buf);
        }
    }
    buf = ":" + _hostname + " 323 " + user.getNickname() + " :End of /LIST";
    _send(user.getSocket(), buf);
}

void IRCServer::_TOPIC(const Message &msg, const User &user)
{
    std::map<std::string, Channel>::iterator ch_it;

    std::string buf_string;
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "TOPIC")
        return;

    if (msg.getParamets().size() == 0)
    {
        buf = ":" + this->_hostname + " 461 " + user.getNickname() + " " + "TOPIC :Not enough parameters";
        _send(user.getSocket(), buf);
        return;
    }

    buf_string = msg.getParamets()[0];
    if (buf_string[0] != '#' && buf_string[0] != '&')
    {

        buf = ":" + this->_hostname + " 403 " + user.getNickname() + " " + buf_string + " :No such channel";
        _send(user.getSocket(), buf);
        return;
    }

    ch_it = this->_channels.find(buf_string);

    if (ch_it != _channels.end())
    {
        if (msg.getParamets().size() == 2)
        {
            ch_it->second.setTopic(msg.getParamets()[1]);

            buf = ":" + user.getNickname() + " TOPIC " + buf_string + " :" + ch_it->second.getTopic();

            _send(user.getSocket(), buf);
            return;
        }
        else if (ch_it->second.getTopic().empty())
        {
            buf = ":" + this->_hostname + " 331 " + user.getNickname() + " " + buf_string + " :No topic is set";
            _send(user.getSocket(), buf);
            return;
        }
        else if (!ch_it->second.getTopic().empty())
        {
            buf = ":" + this->_hostname + " 332 " + user.getNickname() + " " + buf_string + " :" + ch_it->second.getTopic();
            _send(user.getSocket(), buf);
        }
    }
    else
    {
        buf = ":" + this->_hostname + " 403 " + user.getNickname() + " " + buf_string + " :No such channel";
        _send(user.getSocket(), buf);
        return;
    }
}

void IRCServer::_NAMES(const Message &msg, const User &user)
{

    std::map<std::string, Channel>::iterator ch_it;
    std::string message;
    std::string buf;
    std::vector<std::string> buf_string;

    if (utils::toUpper(msg.getCommand()) != "NAMES")
        return;

    for (size_t i = 0; i < msg.getParamets().size(); ++i)
    {
        buf_string.push_back(msg.getParamets()[i]);
        if (!(!buf_string.empty() && (buf_string[i][0] == '#' || buf_string[i][0] == '&')))
            buf_string.pop_back();
    }

    ch_it = _channels.begin();
    if (!buf_string.empty())
    {
        for (size_t i = 0; i < buf_string.size(); ++i)
        {
            std::map<std::string, User *>::const_iterator ch_us_it;
            std::vector<User>::const_iterator ch_chops_it;
            ch_it = this->_channels.find(buf_string[i]);
            if (ch_it != _channels.end())
            {
                ch_us_it = ch_it->second.getUsers().begin();
                message = ":" + this->_hostname + " 353 " + user.getNickname() + " = " + ch_it->second.getName() + " :";
                for (; ch_us_it != ch_it->second.getUsers().end(); ++ch_us_it)
                {
                    ch_chops_it = ch_it->second.getChop(ch_us_it->second->getNickname());
                    if (ch_chops_it != ch_it->second.getChops().end())
                        buf += "@" + ch_us_it->second->getNickname() + " ";
                    else
                        buf += ch_us_it->second->getNickname() + " ";
                }
                _send(user.getSocket(), message + buf);
                message = ":" + this->_hostname + " 366 " + user.getNickname() + " " + ch_it->second.getName() + " :End of /NAMES list";
                _send(user.getSocket(), message);
            }
            else
            {
                message = ":" + this->_hostname + " 366 " + user.getNickname() + " " + buf_string[i] + " :End of /NAMES list";
                _send(user.getSocket(), message);
            }
            buf.clear();
        }
    }
}

void IRCServer::_INVITE(const Message &msg, const User &user)
{

    std::map<std::string, Channel>::iterator ch_it;
    std::map<std::string, User *>::const_iterator us_ch_it;
    std::multimap<std::string, User>::iterator us_it;
    std::string buf;
    std::string buf_string;

    if (utils::toUpper(msg.getCommand()) != "INVITE")
        return;

    if (msg.getParamets().size() < 2)
    {
        buf = ":" + this->_hostname + " 461 " + user.getNickname() + " " + "INVITE :Not enough parameters";
        _send(user.getSocket(), buf);
        return;
    }

    buf_string = msg.getParamets()[1];
    if (buf_string[0] != '#' && buf_string[0] != '&')
        return;

    us_it = this->_users.find(msg.getParamets()[0]);
    if (us_it == this->_users.end() || _channels.empty())
    {
        buf = ":" + this->_hostname + " 401 " + user.getNickname() + " " + msg.getParamets()[0] + " :No such nick or channel";
        _send(user.getSocket(), buf);
        return;
    }

    ch_it = this->_channels.begin();

    us_ch_it = ch_it->second.getUsers().find(user.getNickname());
    if (us_ch_it != ch_it->second.getUsers().end())
    {
        us_ch_it = ch_it->second.getUsers().find(msg.getParamets()[0]);
        if (us_ch_it != ch_it->second.getUsers().end())
        {
            buf = ":" + this->_hostname + " 443 " + user.getNickname() + " " + msg.getParamets()[0] + " :is already on channel" + buf_string;
            _send(user.getSocket(), buf);
        }
        else
        {
            buf = ":" + this->_hostname + " 341 " + user.getNickname() + " " + msg.getParamets()[0] + " " + buf_string;
            _send(user.getSocket(), buf);

            buf = ":" + user.getNickname() + " INVITE " + msg.getParamets()[0] + " :" + buf_string;
            _send(us_it->second.getSocket(), buf);
        }
    }
    else
    {
        buf = ":" + this->_hostname + " 442 " + user.getNickname() + " #" + buf_string + " :You're not on that channel";
        _send(user.getSocket(), buf);
        return;
    }
}

void IRCServer::_QUIT(const Message &msg, User **user)
{
    std::string buf;
    std::map<std::string, Channel>::iterator cit;
    std::map<std::string, User *>::const_iterator uit;

    if (utils::toUpper(msg.getCommand()) != "QUIT")
        return;
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

void IRCServer::_KILL(const Message &msg, User **user)
{
    std::string buf;
    std::map<std::string, Channel>::const_iterator cit;
    std::map<std::string, User *>::const_iterator uit;

    if (utils::toUpper(msg.getCommand()) != "KILL")
        return;
    if (msg.getParamets().size() < 2)
    {
        buf = ":" + _hostname + " 461 KILL :Not enough parameters";
        _send((*user)->getSocket(), buf);
        return;
    }
    if (_operators.find((*user)->getNickname()) == _operators.end())
    {
        buf = ":" + _hostname + " 481 " + (*user)->getNickname() + " :Permission Denied- You're not an IRC operator";
        _send((*user)->getSocket(), buf);
        return;
    }
    if (_users.find(msg.getParamets()[0]) == _users.end())
    {
        buf = ":" + _hostname + " 401 " + msg.getParamets()[0] + " :No such nick";
        _send((*user)->getSocket(), buf);
        return;
    }
    buf = ":" + (*user)->getNickname() + " KILL " + msg.getParamets()[0] + " :" + msg.getParamets()[1];
    _send((*user)->getSocket(), buf);
    buf = ":localhost QUIT :Killed (" + (*user)->getNickname() + " (" + msg.getParamets()[1] + "))";

    // sending quit message to all
    User *killedUser = &_users.find(msg.getParamets()[0])->second;
    User tmp;
    tmp.setNickname(_hostname);
    _QUIT(Message(buf, tmp), &killedUser);
}

void IRCServer::_KICK(const Message &msg, const User &user)
{
    std::string buf;

    if (utils::toUpper(msg.getCommand()) != "KICK")
        return;
    if (msg.getParamets().size() < 2)
    {
        buf = ":" + _hostname + " 461 KILL :Not enough parameters";
        _send(user.getSocket(), buf);
        return;
    }
    if (_channels.find(msg.getParamets()[0]) == _channels.end())
    {
        buf = ":" + _hostname + " 403 " + user.getNickname() + " " + msg.getParamets()[0] + " :No such channel";
        _send(user.getSocket(), buf);
        return;
    }
    Channel &channel = _channels.find(msg.getParamets()[0])->second;
    if (channel.getUsers().find(user.getNickname()) == channel.getUsers().end())
    {
        buf = ":" + _hostname + " 442 " + channel.getName() + " :You're not on that channel";
        _send(user.getSocket(), buf);
        return;
    }
    if (channel.getChop(user.getNickname()) == channel.getChops().end())
    {
        buf = ":" + _hostname + " 482 " + user.getNickname() + " " + channel.getName() + " :You're not channel operator";
        _send(user.getSocket(), buf);
        return;
    }
    if (channel.getUsers().find(msg.getParamets()[1]) == channel.getUsers().end())
    {
        buf = ":" + _hostname + " 441 " + msg.getParamets()[1] + " " + channel.getName() + " :They aren't on that channel";
        _send(user.getSocket(), buf);
        return;
    }
    buf = ":" + user.getNickname() + " KICK " + msg.getParamets()[0] + " " + msg.getParamets()[1];
    _send(channel.getUsers().find(msg.getParamets()[1])->second->getSocket(), buf);
    // removing user
    channel.removeUser(msg.getParamets()[1]);
    // removing channel
    if (channel.getUsers().size() == 0)
    {
        _channels.erase(channel.getName());
        return;
    }
    _sendToChannel(msg.getParamets()[0], buf, msg.getParamets()[1]);
}
