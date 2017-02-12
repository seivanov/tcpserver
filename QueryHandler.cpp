//
// Created by surgeon on 2/9/17.
//

#include "QueryHandler.h"

QueryHandler::QueryHandler(int sockfd) : status(0) {
    this->sockfd = sockfd;
}

void QueryHandler::work(char *buffer) {

    switch(this->status) {

        case 0: {

            if (!strcmp(buffer, "hello")) {

                char tmp[] = "hello\n";
                this->send(tmp);
                this->status = 1;

            }
            break;

        }

        case 1: {

            break;

        }

        default: {

            char tmp[] = "unknown query\n";
            this->send(tmp);
            break;

        }

    }

}

void QueryHandler::send(char *msg) {
    write(this->sockfd, msg, strlen(msg));
}