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
    std::cout << "host:" << hostname << std::endl;
		// zanulentie fd set 

}

IRCServer::~IRCServer() {}

void IRCServer::start() {
	struct protoent *protocol;
	struct sockaddr_in server_adress;

	protocol = getprotobyname("TCP");

	_server_fd = socket(AF_INET, SOCK_STREAM, protocol->p_proto); // need it? or 0?
	if (_server_fd < 0)
		throw std::invalid_argument(strerror(errno));
    fcntl(_server_fd, F_SETFL, O_NONBLOCK);

	server_adress.sin_family = AF_INET;
	server_adress.sin_addr.s_addr = INADDR_ANY;
	server_adress.sin_port = htons(this->_port);

    int yes = 1;
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	int b = bind(this->_server_fd, (struct sockaddr*)&server_adress, sizeof(server_adress));
	if (b < 0)
		throw std::invalid_argument(strerror(errno));
	
	std::cout << "Socket is binding\n"; // del
	std::cout << "Server was created\n"; // del

	// server main_server;
	// main_server.fd_serv = master_socket;

	int l = listen(this->_server_fd, 10);
	if (l < 0) {
		throw std::invalid_argument(strerror(errno));
	}

	// FD_CLR(main_server.client_fds); clear all clients fds.. // need i?
	FD_SET(this->_server_fd, &this->_client_fds);
	this->_max_fd = this->_server_fd;
	
	fd_set select_fds; // just for copy
	select_fds = this->_client_fds; // take the max id from class

	for (int i; select(this->_max_fd + 1, &select_fds, NULL, NULL, NULL) > -1; ){
		i = 0;
		
		while (i < this->_max_fd + 1) {
			
			if (FD_ISSET(i, &select_fds)) {


				// std::cout << RED << "\n" << i << " i inside select fds " << "\n" << END;

				if (i == this->_server_fd) {
					// new client
					struct sockaddr_in temp;

					socklen_t socklen = sizeof(struct sockaddr_in); // temp
					int new_fd = accept(this->_server_fd, (struct sockaddr*)&temp, &socklen); // reinpretet cast ? // eto new client socket
					
					User new_user;
					new_user.setSocket(new_fd);

                    std::pair<std::string, User> myPair(std::string(""),new_user);
                    _users.insert(myPair);

					// if (connect_fd < 0) ...
					std::cout << "connect fd " << new_fd << std::endl; // socketi clientov

					// std::cout << GRE << "new client!!\n" << END;
					if (this->_max_fd < new_fd)
						this->_max_fd = new_fd;
					FD_SET(new_fd, &this->_client_fds);
					// main_server.clients.push_back(make_new_client(new_fd)); // make new user!!
					// std::cout << GRE << "one more joined\n" << END; // add here id - who - add other ..
                    std::cout << RED << "3" << END << std::endl;
					// std::string welcomeMsg = "Welcome to our IRC server! \r\n";
					// send(new_fd, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				}

				else {

					std::cout << "inseide else" << std::endl;

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

					Message msg(buf);
                    std::multimap<std::string, User>::iterator  it;
                    
                    it = _users.begin();
                    while (it != _users.end() && it->second.getSocket() != i)
                        it++;
                    if (it != _users.end())
                    {
                        _PASS(msg, it->second);
                        _PING(msg, it->second);
                    }

					// zdes' budet nash UMNIJ RECIEVE //

                    std::cout << "Number of users : " 
                              << _users.size() + _unloggedUsers.size() << std::endl;


					it = _users.begin(); // 
					for (; it != _users.end(); ++it)
                    {
						// _send(it->second.getSocket(), std::string(buf));
					}
				}
			}
			i++;
		}
		select_fds = this->_client_fds;
		// FD CLEAR!!
	}
	std::cout << "smth bad" << std::endl;
	exit(11);


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
    std::cout << GRE << "msg len : " << END << buf.length() << std::endl;
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
    std::cout << YEL << "msg len : " << END << buf_delim.length() << std::endl;
    std::cout << YEL << "msg     : " << END << buf_delim << std::endl;
    std::cout << YEL << "▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲" << END << std::endl;
    return (bytes == -1 ? false : true);
}

void    IRCServer::_removeUser(int sockfd)
{
    std::multimap<std::string, User>::iterator  it;
    std::vector<User>::iterator                 it_u;

    it = _users.begin();
    while (it != _users.end() && it->second.getSocket() != sockfd)
        it++;
    if (it != _users.end())
        _users.erase(it);

    it_u = _unloggedUsers.begin();
    while (it_u != _unloggedUsers.end() && it_u->getSocket() != sockfd)
        it_u++;
    if (it_u != _unloggedUsers.end())
        _unloggedUsers.erase(it_u);
}

void IRCServer::_PRIVMSG(const Message &msg, const User &usr) {
	// msg.cmd == PRIVMSG -> budet ran'she
	
	// check if client exists ( USR ) // nickname
	// sam sebe otpravit' miozhet?
	// send send( int sockfd - komu, const std::string &buf - soobshenie//
	
	// proverka na valid imeni group budet v sozdanii grouppi !
	
	std::map<std::string, User>::iterator us_it;
	us_it = this->_users.begin();

	us_it = this->_users.find(msg.getParamets()[0]);
	std::cout << "YES\n";
	if (us_it != this->_users.end()) {
		_send(usr.getSocket(), msg.getParamets().back());
	}
	else {
		_send(usr.getSocket(), "Not work");
	}
}

void    IRCServer::_PASS( const Message &msg, User &user )
{
    std::string	buf;

    if (msg.getCommand() != "PASS")
        return ;
    std::cout << RED << user.isPassworded() << END << std::endl;
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
	// 1. msg.getParamets()[0] - eto nazvanie gruppi?

	std::map<std::string, Channel>::iterator ch_it;
	ch_it = this->_channels.find(msg.getParamets()[0]); // najti group
	
	if (ch_it != this->_channels.end()) {
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

		Channel new_ch(msg.getParamets()[0]);
		this->_channels.insert(std::make_pair(new_ch._name, new_ch));
		// send msg that new channel is created
		new_ch.addUser(usr);
		new_ch.addChop(usr);
		// std::cout to socket: name .. - is chop now and to server

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