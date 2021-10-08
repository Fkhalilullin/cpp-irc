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

std::vector<std::string> split(const std::string &str, char delimeter)
{
    std::vector<std::string> result;
    std::istringstream sstream(str);
    std::string tmp;
    while (std::getline(sstream, tmp, delimeter))
        result.push_back(tmp);
    return (result);
}



int main(int argc, char *argv[]) {

	auto vec = split(std::string("CMD arg1 arg2"), ' ');
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::cout << vec[i] << std::endl;
	}
	return 0;
	
	if (argc != 3) {
		std::cout << RED << "valid params need" << END << std::endl;
		return 1;
	}

	unsigned int port = std::atoi(argv[1]);
	std::string pass(argv[2]);
	IRCServer serv(port, pass);
	serv.start();


	return 0;
}

