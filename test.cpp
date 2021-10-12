/*
** selectserver.c — сервер многопользовательского чата
*/#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT «9034»   // порт, который мы слушаем

// получаем sockaddr, IPv4 или IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}

return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master;    // главный сет дескрипторов
    fd_set read_fds;  // временный сет дескрипторов для select()
    int fdmax;        // макс. число дескрипторов

    int listener;     // дескриптор слушающего сокета
    int newfd;        // дескриптор для новых соединений после accept()
    struct sockaddr_storage remoteaddr; // адрес клиента
    socklen_t addrlen;

    char buf[256];    // буфер для данных клиента
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // для setsockopt() SO_REUSEADDR, ниже
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // очищаем оба сета
    FD_ZERO(&read_fds);

    // получаем сокет и биндим его
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, «selectserver: %sn», gai_strerror(rv));
    exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
    continue;
    }

    // избегаем ошибки «address already in use»
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
    close(listener);
    continue;
    }

    break;
    }

    // если мы попали сюда, значит мы не смогли забиндить сокет
    if (p == NULL) {
    fprintf(stderr, «selectserver: failed to bindn»);
    exit(2);
    }

    freeaddrinfo(ai); // с этим мы всё сделали

    // слушаем
    if (listen(listener, 10) == —1) {
    perror(«listen»);
    exit(3);
    }

    // добавляем слушающий сокет в мастер-сет
    FD_SET(listener, &master);

    // следим за самым большим номером дескриптора
    fdmax = listener; // на данный момент это этот

    // главный цикл
    for(;;)
    {
        read_fds = master; // копируем его
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == —1)
        {
            perror(«select»);
            exit(4);
        }

        // проходим через существующие соединения, ищем данные для чтения
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            { // есть!
                if (i == listener)
                {
                    // обрабатываем новые соединения
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                    (struct sockaddr *)&remoteaddr,
                    &addrlen);

                    if (newfd == —1)
                    {
                        perror(«accept»);
                    }
                    else
                    {
                        FD_SET(newfd, &master); // добавляем в мастер-сет
                        if (newfd > fdmax)
                        {    // продолжаем отслеживать самый большой номер дескиптора
                            fdmax = newfd;
                        }
                        printf(«selectserver: new connection from %s on «socket %dn»,
                                inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                                newfd);
                    }
                }
                else
                {
                    // обрабатываем данные клиента
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
                    {
                        // получена ошибка или соединение закрыто клиентом
                        if (nbytes == 0)
                        {
                            // соединение закрыто
                            printf(«selectserver: socket %d hung upn», i);
                        }
                        else
                        {
                            perror(«recv»);
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // удаляем из мастер-сета
                    }
                    else
                    {
                        // у нас есть какие-то данные от клиента
                        for(j = 0; j <= fdmax; j++)
                        {
                            // отсылаем данные всем!
                            if (FD_ISSET(j, &master))
                            {
                                //   кроме слушающего сокета и клиента, от которого данные пришли
                                if (j != listener && j != i)
                                {
                                    if (send(j, buf, nbytes, 0) == —1)
                                    {
                                        perror(«send»);
                                    }
                                }
                            }
                        }
                    }
                } // Закончили обрабатывать данные от клиента
            } // Закончили обрабатывать новое входящее соединение
        } // Закончили цикл по дескрипторам
    } // Закончили for(;;) — А вы-то думали, что это никогда не кончится!

    return 0;
}