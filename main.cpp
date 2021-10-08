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

#include "IRCServer.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "User.hpp"

#define RED "\033[31m"
#define GRE "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define END "\033[37m"



// int main(int argc, char *argv[]) {
int main() {

	// if (argc != 3) {
	// 	std::cout << RED << "valid params need" << END << std::endl;
	// 	return 1;
	// }

	// unsigned int port = std::atoi(argv[1]);
	// std::string pass(argv[2]);
	// IRCServer serv(port, pass);
	// serv.start();

	User kek;
	kek.setNickname("kke132");

	User lol;
	lol.setNickname("lll132");

	Channel cha("new chanel");
	cha.addUser(kek);
	cha.addUser(lol);
	cha.addChop(kek);
	// cha.addChop(kek);

	std::cout << std::endl;
	
	// cha.removeUser("kke132");
	cha.channel_info();
	

	
	
	// std::cout << "users" << std::endl;

	// std::map<std::string, User*> us = cha.getUsers();
	// std::map<std::string, User*>::iterator itus;
	// itus = us.begin();
	// for (; itus != us.end(); itus++) {
	// 	std::cout << itus->first << std::endl;
	// }
	
	// //
	// std::cout << "chops" << std::endl;

	// std::map<std::string, User*> chps = cha.getChops();
	// std::map<std::string, User*>::iterator itchop;
	// itchop = chps.begin();
	// for (; itchop != chps.end(); itchop++) {
	// 	std::cout << itchop->first << std::endl;
	// }

	return 0;
}

