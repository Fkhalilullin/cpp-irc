#ifndef SERV_HPP
#define SERV_HPP

#include <iostream>
#include <vector>

class client {
	private:


	public:
		int fd_client;
		std::string nick;
		std::string channel;
		int id;

		// make here static int for id
		// color of msg
		// std::string username; ?
};

class server {
	private:

	public:
		int fd_serv;
		int max_fd;
		char *buff; // BUFF SIZE;

		std::vector<client*> clients; // make map - id - clent maybe
		// std::vector<channels*> clients;


		fd_set client_fds;
};

class request {
	// /\/here should be join, change name and other ...
};


#endif