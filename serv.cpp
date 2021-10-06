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

#include "serv.hpp"
  
#define PORT 10000
#define BUFF_SIZE 1024

client* make_new_client(int input_socket_id) { // made him name
	client *new_client = new client; // mozno bez ssilki
	
	// make norm constructor
	// new_client->id = .
	// new_client->color msg= .
	new_client->fd_client = input_socket_id;
	// new_client->channel = NULL; // inside what chanels it is?


	return new_client;
}
 
int main(int argc, char *argv[]) {
	// need i argc, argv?

	// ac != 1 || 4 
	// zabronirovat' porti// net 8080 || <1024 || > 65535 -> make them ints from chars

	int opt = true;

	struct protoent *protocol;
	protocol = getprotobyname("TCP");
	
	int master_socket;
	struct sockaddr_in server_adress;

	master_socket = socket(AF_INET, SOCK_STREAM, protocol->p_proto); // need it? or 0?
	if (master_socket < 0)
		std::cout << "NO!\n";
	std::cout << "socket ok\n";

	// fill struct
	server_adress.sin_family = AF_INET;
	server_adress.sin_addr.s_addr = INADDR_ANY;
	server_adress.sin_port = htons(PORT); // get from argv[1]
	std::cout << "adress filled ok\n";

	int b = bind(master_socket, (struct sockaddr *)&server_adress, sizeof(server_adress)); // reinpretet cast?
	if (b < 0)
		std::cout << "NO!\n";
	std::cout << "bind ok\n";
	std::cout << "server created\n";

	server main_server;
	main_server.fd_serv = master_socket;

	int l = listen(main_server.fd_serv, 5);
	// if (l < 0) ...
	std::cout << "listening socket\n";
	
	// FD_CLR(main_server.client_fds); clear all clients fds.. // need i?
	FD_SET(main_server.fd_serv, &main_server.client_fds);
	main_server.max_fd = main_server.fd_serv; // nachinaem s odnogo tol'ko servera -> potom bydut clients
	std::cout << main_server.max_fd << std::endl;
	
	fd_set select_fds; // just for copy
	select_fds = main_server.client_fds; // take the max id from class

	for (int i; select(main_server.max_fd + 1, &select_fds, NULL, NULL, NULL) > -1; ) {
		i = 0;
		
		while (i < main_server.max_fd + 1) {
			
			if (FD_ISSET(i, &select_fds)) {
				if (i == main_server.fd_serv) {
					// new client

					int connect_fd;
					struct sockaddr_in temp;
					socklen_t socklen;

					socklen = sizeof(struct sockaddr_in); // temp
					connect_fd = accept(main_server.fd_serv, (struct sockaddr *)&temp, &socklen); // reinpretet cast ?
					// if (connect_fd < 0) ...

					std::cout << "new client!!\n";
					if (main_server.max_fd < connect_fd)
						main_server.max_fd = connect_fd;
					FD_SET(connect_fd, &main_server.client_fds);
					main_server.clients.push_back(make_new_client(connect_fd));
					std::cout << "one more joined\n"; // add here id - who - add other ..

				}

				else {
					// msg handling
					// easy part ...


				}

			}

			i++;
		}
		select_fds = main_server.client_fds;
	}
	std::cout << "smth bad" << std::endl;
	exit(11);


	return 0;
}
