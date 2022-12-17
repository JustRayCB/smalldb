#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cstddef>
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


template<class typeValue>
int checkRecv(int &server, typeValue *results, unsigned long &&size){
    int bytesRead = recv(server, results, size, 0);
    if (bytesRead == -1) {
        std::cout << "Unknown error has occured while receiving data" << std::endl;
        return 1;
    }else if (bytesRead == 0) {
        std::cout << "The server is not available anymore or you have been disconnected "
            "due to innactivity can not receive data" << std::endl;
        return 1;
    }
    return 0;
}
int checkSend(int &server, std::string &results){
    int bytesSent = send(server, results.data(), results.size()+1, 0);
    if (bytesSent < 0) {
        if (errno == EPIPE) {
            //server disconnected
            std::cout << "The server is disconnected or you have been disconnected"
                " due to innactivity can not send data" << std::endl;
            return 1;
        }else {
            std::cout << "Unknown error while sending data" << std::endl;
            return 1;
        }
    }
    return 0;
}

int printResult(int &server){
    int size;
    std::string ok = "ok";
    if(checkRecv(server, &size, sizeof(size))){
        return 1;
    }
    size = ntohs(size);
    char *results = new char[size];
    if(checkRecv(server, results, size)){
        return 1;
    }
    if (checkSend(server, ok)) {
        return 1;
    }
    std::cout << results;
    delete [] results;
    results = nullptr;
    return 0;

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
            if(checkSend(server, msg)){
                break;
            }
            if(checkRecv(server, &valueSize, sizeof(valueSize))){
                break;
            }
            valueSize = ntohl(valueSize);
            for (int idx=0; idx < valueSize; idx++) {
                if (not sigint) {break;}
                if (printResult(server)){break;}
            }
        }
    }
    close(server);
    return 0;

}
