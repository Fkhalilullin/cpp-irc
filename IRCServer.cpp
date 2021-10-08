#include "IRCServer.hpp"

IRCServer::IRCServer(unsigned int port, std::string pass) :
	_port(port),
	_password(pass) {

		// zanulentie fd set 

}

IRCServer::~IRCServer() {}

void IRCServer::start() {

	int opt = true; // zachem??
	struct protoent *protocol;
	struct sockaddr_in server_adress;

	protocol = getprotobyname("TCP");

	this->_server_fd = socket(AF_INET, SOCK_STREAM, protocol->p_proto); // need it? or 0?
	if (this->_server_fd < 0)
		throw std::invalid_argument(strerror(errno));

	server_adress.sin_family = AF_INET;
	server_adress.sin_addr.s_addr = INADDR_ANY;
	server_adress.sin_port = htons(this->_port);

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
	// int s = select(main_server.max_fd + 1, &select_fds, NULL, NULL, NULL);
	// std::cout << "select number " << s << std::endl;

	for (int i; select(this->_max_fd + 1, &select_fds, NULL, NULL, NULL) > -1; ) {
		i = 0;
		
		while (i < this->_max_fd + 1) {
			
			// std::cout << i << " in while\n";

			if (FD_ISSET(i, &select_fds)) {


				// std::cout << RED << "\n" << i << " i inside select fds " << "\n" << END;

				if (i == this->_server_fd) {
					// new client
					struct sockaddr_in temp;

					socklen_t socklen = sizeof(struct sockaddr_in); // temp
					int new_fd = accept(this->_server_fd, (struct sockaddr*)&temp, &socklen); // reinpretet cast ? // eto new client socket
					
					User new_user;
					new_user.setSocket(new_fd);
					_unloggedUsers.push_back(new_user);
					_users.insert(std::make_pair(std::to_string(new_fd), new_user));

					// if (connect_fd < 0) ...
					std::cout << "connect fd " << new_fd << std::endl; // socketi clientov

					// std::cout << GRE << "new client!!\n" << END;
					if (this->_max_fd < new_fd)
						this->_max_fd = new_fd;
					FD_SET(new_fd, &this->_client_fds);
					// main_server.clients.push_back(make_new_client(new_fd)); // make new user!!
					// std::cout << GRE << "one more joined\n" << END; // add here id - who - add other ..

					std::string welcomeMsg = "Welcome to our IRC server! \r\n";
					send(new_fd, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				}

				else {

					std::cout << "inseide else" << std::endl;
					char buf[1024]; // buff size
					memset(buf, '\0', sizeof(buf));
					int bytes_in = recv(i, buf, sizeof(buf), 0);
					
					if (bytes_in < 1) {
						close(i);
						FD_CLR(i, &this->_client_fds);
					}
					
					// Message msg;
					// msg._parse(buf);

					// zdes' budet nash UMNIJ RECIEVE //

					std::cout << "posle if" << std::endl;
					// else 
					std::cout << "MAP SIZE = " << _users.size() << std::endl;


					std::vector<User>::iterator uit = _unloggedUsers.begin(); // 
					for (; uit != _unloggedUsers.end(); ++uit) {
						std::cout << "Sended to: " << uit->_socket << " " << buf << std::endl;
						// send(uit->_socket, buf, bytes_in, 0);
					}

					// EXEC


					//
					// std::map<std::string, User>::iterator it = _users.begin();
					// for (; it != _users.end(); ++it)
					// {
					// 	send(it->second._socket, buf, bytes_in, 0);
					// 	std::cout << "Sended to: " << it->second._socket << " " << buf << std::endl;
					// }
					//
					for (int k = 0; k < this->_max_fd + 1; k++) {

						int out_socket = k;
						if (out_socket != this->_server_fd && out_socket != i) {
							
							std::ostringstream ss;
							ss << "guy in the chat # " << i << ": ";
							std::string string_to_send = ss.str();
							send(out_socket, string_to_send.c_str(), string_to_send.size() + 1, 0);
							send(out_socket, buf, bytes_in, 0);
						}
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
