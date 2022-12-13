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

#define PORT (28772)
#define SOCKETERROR (-1)
#define BUFFSIZE (200)

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


int main(int argc, const char* argv[]){
    if (argc <2) {
        std::cout << "Sorry please enter an ip address to connect" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    signal(SIGINT, signalHandler);
        
    int client = check(socket(AF_INET, SOCK_STREAM, 0), "Client: failed to create socket");
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, argv[argc-1], &serv_addr.sin_addr);
        
    check(connect(client, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "Connection failed");
    
    std::string msg;
    int valueSize;

    while (std::getline(std::cin, msg) and sigint){
        if (msg != "") {
            msg += '\0';
            send(client, msg.c_str(), msg.size(), 0);
            recv(client, &valueSize, sizeof(valueSize), 0);
            valueSize = ntohl(valueSize);
            int ret=0;
            for (int idx=0; idx < valueSize; idx++) {
                int size;
                recv(client, &size, sizeof(size), 0);
                //check(read(client, &size, sizeof(size)), "Read failed 2");
                size = ntohs(size);
                //std::cout << "I received the size for value : " << size << std::endl;
                char *results = new char[size];
                ret += recv(client, results, size, 0);
                send(client, "ok\0", 3, 0);
                results[size-1] = '\0';
                std::cout << results;
                delete [] results;
                results = nullptr;

            }
        }
      
    }
    close(client);
    return 0;

}
