#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <fcntl.h>

#include <errno.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "QueryHandler.h"
#include "Crypto.h"
#include "lib.h"

#define DEBUG true

void *connection_handler(void *);
void message_exec(char *, int);
void GenKeys(char secret[]);

int main() {

    int server_sockfd, client_sockfd, check;
    unsigned int server_len, client_len;
    struct sockaddr_in server_address, client_address;
    pthread_t sniffer_thread;

#ifndef DEBUG
    if(!fork()) {
#endif

        /* open socket */
        server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if(!server_sockfd) {
            printf("error open socket\n");
            return 1;
        }

        int flag = 1;
        setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));

        /* socket setup */
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons(9734);
        server_len = sizeof(server_address);
        check = bind(server_sockfd, (struct sockaddr *) &server_address, server_len);

        if(check == -1) {
            printf("error bind %d\n", errno);
            return 1;
        }

        /* connecting socket */
        check = listen(server_sockfd, 128);

        if(check == -1) {
            printf("error listen %d\n", errno);
            return 1;
        }

        while (1) {

            printf("server waiting\n");

            client_len = sizeof(client_address);
            client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);

            if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *) &client_sockfd) < 0) {

                perror("could not create thread");
                return 1;

            } else {

                pthread_detach(sniffer_thread);

            }

        }

#ifndef DEBUG
    } else
        return 0;
#endif

}

void *connection_handler(void *client_sockfd)
{

    const unsigned int MAXMSG = 1024;

    int sockfd = *((int *)client_sockfd);
    char buffer[MAXMSG];
    ssize_t nbytes;
    char *passwd = _randomString(20);

    QueryHandler *query = new QueryHandler(sockfd);
    Crypto *crypt = new Crypto();

    crypt->GenKeys(passwd);

    while(1) {

        nbytes = read(sockfd, buffer, MAXMSG-1);
        if (nbytes < 0 || nbytes == 0) {
            break;
        } else {

            buffer[nbytes] = '\0';
            char *p = strtok(buffer, "\r\n");
            if(p) {
                fprintf(stderr, "Server: got message: `%s'\n", p);
                query->work(p);
            }

        }

    }

    delete query;
    delete crypt;
    return (void*)0;

}