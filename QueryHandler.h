//
// Created by surgeon on 2/9/17.
//

#ifndef WEBSERVER_QUERYHANDLER_H
#define WEBSERVER_QUERYHANDLER_H

#include <string.h>
#include <unistd.h>

class QueryHandler {

    private:

        int sockfd;
        int status;

    public:

        QueryHandler(int sockfd);
        void work(char*);
        void send(char*);

};


#endif //WEBSERVER_QUERYHANDLER_H
