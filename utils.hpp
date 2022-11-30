#ifndef _UTILS_HPP
#define _UTILS_HPP


struct Client{
    const unsigned id;
    const int clientSocket;
};

void signalHandler(int signum);

int check(int exp, const char *msg);

void *handleConnection(void *client);

#endif
