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
					int connect_fd = accept(this->_server_fd, (struct sockaddr*)&temp, &socklen); // reinpretet cast ? // eto new client socket
					// if (connect_fd < 0) ...
					std::cout << "connect fd " << connect_fd << std::endl; // socketi clientov

					// std::cout << GRE << "new client!!\n" << END;
					if (this->_max_fd < connect_fd)
						this->_max_fd = connect_fd;
					FD_SET(connect_fd, &this->_client_fds);
					// main_server.clients.push_back(make_new_client(connect_fd)); // make new user!!
					// std::cout << GRE << "one more joined\n" << END; // add here id - who - add other ..

					std::string welcomeMsg = "Welcome to our IRC server! \r\n";
					send(connect_fd, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				}

				else {
					// msg handling
					// easy part ...

					// std::cout << "i inside else: " << i << std::endl;
					char buf[1024]; // buff size
					memset(buf, '\0', sizeof(buf)); // \0 ??
					int bytes_in = recv(i, buf, sizeof(buf), 0); // i ??
					// if (bytes_in < 1) // -> drop the client
					// { close(i); // is it right for mac?
					// FD_CLR(i, &main_server.client_fds); }
					// else 

					// HERE WOULD BE REQUEST OR MSG

					for (int k = 0; k < this->_max_fd; k++) {
						int out_socket = k;
						if (out_socket != this->_server_fd && out_socket != i) {
							
							// std::cout << "chto-to proishodit\n"; // otpravlyet po kol-vo fds in set
							
							// send(out_socket, temp, 3, 0);
							// std::string sstm; 
							// sstm << name << age;
							// result = sstm.str()
							
							// std::string ss;
							// ss = "guy in the chat # " + char(i + 48);  // + i
							// const char *str = ss.c_str();
							// send(out_socket, str, strlen(str), 0);
							send(out_socket, buf, bytes_in, 0);

							// std::ostringstream ss;
							// ss << "guy in the chat # " << i << ": ";
							// std::string string_to_send = ss.str();
							// send(out_socket, string_to_send.c_str(), string_to_send.size() + 1, 0);
							// send(out_socket, buf, bytes_in, 0);
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
