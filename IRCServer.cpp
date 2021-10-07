#include "IRCServer.hpp"

IRCServer::IRCServer(unsigned int port, std::string pass) :
	_port(port),
	_password(pass) {

}

IRCServer::~IRCServer() {}

void IRCServer::start() {

	int opt = true; // zachem??
	struct protoent *protocol;
	struct sockaddr_in server_adress;

	protocol = getprotobyname("TCP");

	this->_server_fd = socket(AF_INET, SOCK_STREAM, protocol->p_proto); // need it? or 0?
	if (this->_server_fd < 0)
		throw std::invalid_argument("Socket hasn't been created\n");

	server_adress.sin_family = AF_INET;
	server_adress.sin_addr.s_addr = INADDR_ANY;
	server_adress.sin_port = htons(this->_port);

	int b = bind(this->_server_fd, (struct sockaddr*)&server_adress, sizeof(server_adress));
	if (b < 0)
		throw std::invalid_argument("Socket hasn't been binding\n");
	std::cout << "Socket is binding\n";
	std::cout << "Server was created\n";


}
