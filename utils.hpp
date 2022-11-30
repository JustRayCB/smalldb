#ifndef _UTILS_HPP
#define _UTILS_HPP



void signalHandler(int signum);

int check(int exp, const char *msg);

void *handleConnection(void *client);

#endif
