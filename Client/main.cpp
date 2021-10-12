#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <iostream>

#define PORT "7770" // Порт, к которому подключается клиент

#define MAXDATASIZE 100 // максимальное число байт, принимаемых за один раз

#define RED "\033[31m"
#define GRE "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define END "\033[37m"

// получение структуры sockaddr, IPv4 или IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr,"usage: client hostnamen");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %sn", gai_strerror(rv));
        return 1;
    }

    // Проходим через все результаты и соединяемся к первому возможному
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
        perror("client: socket");
        continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connectn");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    printf("client: connecting to %sn", s);

    freeaddrinfo(servinfo); // эта структура больше не нужна


    // while (21)
    // {
        

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl;

        std::string sbuf;
        sbuf = "NICK PJLK\r\n";
        send(sockfd, sbuf.c_str(), sbuf.length(), 0);
        sbuf = "USER PJLK n1 n2 n3\r\n";
        send(sockfd, sbuf.c_str(), sbuf.length(), 0);

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        std::cout << YEL << buf << END << std::endl << std::endl;
        // printf("client: received ‘%s’n",buf);
        

        // usleep(2000);

    // }

    close(sockfd);

    return 0;
}