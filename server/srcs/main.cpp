#include <iostream>

#include "../includes/IRCServer.hpp"
#include "../includes/Message.hpp"
#include "../includes/Channel.hpp"
#include "../includes/User.hpp"

#define RED "\033[31m"
#define END "\033[37m"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << RED << "valid params need" << END << std::endl;
		return 1;
	}

	unsigned int port = std::atoi(argv[1]);
	std::string pass(argv[2]);

	try
	{
		IRCServer serv(port, pass);
		serv.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << e.what() << END << std::endl;
	}
	return 0;
}
