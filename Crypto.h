//
// Created by Станислав on 10.02.17.
//

#ifndef WEBSERVER_CRYPTO_H
#define WEBSERVER_CRYPTO_H

#include <iostream>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <fcntl.h>
#include <unistd.h>

class Crypto {

    public:

        void GenKeys(char secret[]);
        void Encrypt();
        void Decrypt(char secret[]);

};


#endif //WEBSERVER_CRYPTO_H
