#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SIZE 50
#define NUM_CLIENT 5

void *connection_handler(void *socket_desc);

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

        char *ch;

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

    int sockfd = *((int *)client_sockfd);

    printf("from thread \n");

    while(1) {

        char *ch;

        if(read(sockfd, &ch, 1) == 0)
            break;

        printf("%c , %d\n",ch,ch);

    }

    printf("end thread \n");

}