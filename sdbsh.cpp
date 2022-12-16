#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>

#include "common.hpp"

int sigint = 1;
int check(int exp, const char *msg){
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}


void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << std::endl << "Handling SIGINT ..." << std::endl;
        std::cout<< "Please press Enter to terminate the program" << std::endl;
        sigint = 0;
    }
}

void printResult(int &server){
    int size;
    recv(server, &size, sizeof(size), 0);
    size = ntohs(size);
    char *results = new char[size];
    recv(server, results, size, 0);
    send(server, "ok\0", 3, 0);
    //results[size-1] = '\0';
    std::cout << results;
    delete [] results;
    results = nullptr;

}

int main(int argc, const char* argv[]){
    if (argc <2) {
        std::cout << "Sorry please enter an ip address to connect" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    signal(SIGINT, signalHandler);
        
    int server = check(socket(AF_INET, SOCK_STREAM, 0), "Client: failed to create socket");
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, argv[argc-1], &serv_addr.sin_addr);
        
    check(connect(server, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "Connection failed");
    
    std::string msg;
    int valueSize;

    while (std::getline(std::cin, msg) and sigint){
        if (msg != "") {
            msg += '\0';
            send(server, msg.c_str(), msg.size(), 0);
            recv(server, &valueSize, sizeof(valueSize), 0);
            valueSize = ntohl(valueSize);
            for (int idx=0; idx < valueSize; idx++) {
                printResult(server);
            }
        }
      
    }
    close(server);
    return 0;

}
