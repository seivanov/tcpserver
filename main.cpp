#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "QueryHandler.h"

void *connection_handler(void *);
void message_exec(char *, int);

int main() {

    int server_sockfd, client_sockfd;
    unsigned int server_len, client_len;
    struct sockaddr_in server_address, client_address;
    pthread_t sniffer_thread;

    /* open socket */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* socket setup */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    /* connecting socket */
    listen (server_sockfd, 128);
    while(1) {

        printf("server waiting\n");

        client_len = sizeof (client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) &client_sockfd) < 0) {

            perror("could not create thread");
            return 1;

        } else {

            pthread_detach(sniffer_thread);

        }

    }

}

void *connection_handler(void *client_sockfd)
{

    const unsigned int MAXMSG = 1024;

    int sockfd = *((int *)client_sockfd);
    char buffer[MAXMSG];
    ssize_t nbytes;

    while(1) {

        nbytes = read(sockfd, buffer, MAXMSG-1);
        if (nbytes < 0 || nbytes == 0) {
            break;
        } else {

            buffer[nbytes] = '\0';
            char *p = strtok(buffer, "\r\n");
            if(p) {
                fprintf(stderr, "Server: got message: `%s'\n", p);
                message_exec(p, sockfd);
            }

        }

    }

}

void message_exec(char *msg, int sockfd) {

    if(!strcmp(msg, "test")) {

        fprintf(stderr, "Server: UPS: \n");
        char b[] = "hello test";
        write(sockfd, &b, strlen(b));

    }

}