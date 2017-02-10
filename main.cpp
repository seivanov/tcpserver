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

//using namespace std;

void *connection_handler(void *);
void message_exec(char *, int);
void GenKeys(char secret[]);

int main() {

    int server_sockfd, client_sockfd, check;
    unsigned int server_len, client_len;
    struct sockaddr_in server_address, client_address;
    pthread_t sniffer_thread;

    //if(!fork()) {

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

    /*
    } else
        return 0;
    */

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

    return (void*)0;

}

void message_exec(char *msg, int sockfd) {

    if(!strcmp(msg, "test")) {

        fprintf(stderr, "Server: UPS: \n");
        char b[] = "hello test";
        write(sockfd, &b, strlen(b));

    }

}

void GenKeys(char secret[]){
    /* указатель на структуру для хранения ключей */
    RSA * rsa = NULL;
    unsigned long bits = 1024; /* длина ключа в битах */
    FILE * privKey_file = NULL, *pubKey_file = NULL;
    /* контекст алгоритма шифрования */
    const EVP_CIPHER *cipher = NULL;
    /*Создаем файлы ключей*/
    privKey_file = fopen("private.key", "wb");
    pubKey_file = fopen("public.key", "wb");
    /* Генерируем ключи */
    rsa = RSA_generate_key(bits, RSA_F4, NULL, NULL);
    /* Формируем контекст алгоритма шифрования */
    cipher = EVP_get_cipherbyname("bf-ofb");
    /* Получаем из структуры rsa открытый и секретный ключи и сохраняем в файлах.
    * Секретный ключ шифруем с помощью парольной фразы
    */
    PEM_write_RSAPrivateKey(privKey_file, rsa, cipher, NULL, 0, NULL, secret);
    PEM_write_RSAPublicKey(pubKey_file, rsa);
    /* Освобождаем память, выделенную под структуру rsa */
    RSA_free(rsa);
    fclose(privKey_file);
    fclose(pubKey_file);
    std::cout << "Ключи сгенерированы и помещены в папку с исполняемым файлом" << std::endl;
}

void Encrypt(){
    /* структура для хранения открытого ключа */
    RSA * pubKey = NULL;
    FILE * pubKey_file = NULL;
    unsigned char *ctext, *ptext;
    int inlen, outlen;
    /* Считываем открытый ключ */
    pubKey_file = fopen("public.key", "rb");
    pubKey = PEM_read_RSAPublicKey(pubKey_file, NULL, NULL, NULL);
    fclose(pubKey_file);

    /* Определяем длину ключа */
    int key_size = RSA_size(pubKey);
    ctext = (unsigned char *)malloc(key_size);
    ptext = (unsigned char *)malloc(key_size);
    OpenSSL_add_all_algorithms();

    int out = open("rsa.file", O_CREAT | O_TRUNC | O_RDWR, 0600);
    int in = open("in.txt", O_RDWR);
    /* Шифруем содержимое входного файла */
    while (1) {
        inlen = read(in, ptext, key_size - 11);
        if (inlen <= 0) break;
        outlen = RSA_public_encrypt(inlen, ptext, ctext, pubKey, RSA_PKCS1_PADDING);
        if (outlen != RSA_size(pubKey)) exit(-1);
        write(out, ctext, outlen);
    }
    std::cout << "Содержимое файла in.txt было зашифровано и помещено в файл rsa.file" << std::endl;
}

void Decrypt(char secret[]){
    RSA * privKey = NULL;
    FILE * privKey_file;
    unsigned char *ptext, *ctext;
    int inlen, outlen;

    /* Открываем ключевой файл и считываем секретный ключ */
    OpenSSL_add_all_algorithms();
    privKey_file = fopen("private.key", "rb");
    privKey = PEM_read_RSAPrivateKey(privKey_file, NULL, NULL, secret);

    /* Определяем размер ключа */
    int key_size = RSA_size(privKey);
    ptext = (unsigned char *)malloc(key_size);
    ctext = (unsigned char *)malloc(key_size);

    int out = open("out.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);
    int in = open("rsa.file", O_RDWR);

    /* Дешифруем файл */
    while (1) {
        inlen = read(in, ctext, key_size);
        if (inlen <= 0) break;
        outlen = RSA_private_decrypt(inlen, ctext, ptext, privKey, RSA_PKCS1_PADDING);
        if (outlen < 0) exit(0);
        write(out, ptext, outlen);
    }
    std::cout << "Содержимое файла rsa.file было дешифровано и помещено в файл out.txt" << std::endl;

}