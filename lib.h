//
// Created by Станислав on 10.02.17.
//

#ifndef LIB_H
#define LIB_H

#include <time.h>
#include <stdlib.h>

unsigned int _random() {


    return (48 + rand() % 90);

}

char *_randomString(int len) {

    char *p = new char[len];

    srand(time(NULL));
    for(int i = 0; i < len; i++)
        p[i] = _random();

    return p;

}

#endif //LIB_H
