#include <iostream>
#include <signal.h>
#include <unistd.h>

#include "utils.hpp"

#define SOCKETERROR (-1)


void signalHandler(int signum) {
   if (signum == SIGINT) { //^C
                           //Save and end programm
        //std::cout << "Handling SIGINT signal ... "<< std::endl;
        //fclose(stdin); // Close stdin to get out of the waiting for input
    }
}


int check(int exp, const char *msg){
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}
