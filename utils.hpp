#ifndef _UTILS_HPP
#define _UTILS_HPP

#include "db.hpp"

struct Client{
    const int socket;
    database_t *db;
};


int check(int exp, const char *msg);

void *handleConnection(void *client);

#endif
