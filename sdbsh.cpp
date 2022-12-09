#include <cstdio>
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

int sigint =1;

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
        //fclose(stdin);
        sigint = 0;
    }
}



int main(){
    
    signal(SIGINT, signalHandler);
    int client = check(socket(AF_INET, SOCK_STREAM, 0), "Client: failed to create socket");
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        
    check(connect(client, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "Connection failed");
    
    std::string msg;
    int valueSize;

    while (sigint and msg != "exit"){
        std::cout << "Please enter a command : ";
        std::getline(std::cin, msg);
        if (msg != "") {
            msg += '\0';
            //send(client, msg.c_str(), msg.size(), 0);
            if (send(client, msg.c_str(), msg.size(), 0) < 0){
                std::cout << "Send Failed, server is closed" << std::endl;
                break;
            }
            //recv(client, &valueSize, sizeof(valueSize), 0);
            if (recv(client, &valueSize, sizeof(valueSize), 0) < 0){
                std::cout << "Recv Failed, server is closed" << std::endl;
                break;
            }
            valueSize = ntohl(valueSize);
            int ret=0;
            for (int idx=0; idx < valueSize; idx++) {
                int size;
                recv(client, &size, sizeof(size), 0);
                size = ntohl(size);
                char *results = new char[size];
                ret += recv(client, results, size, 0);
                std::cout << results;
                delete [] results;

            }
        
        }
    }
    std::cout << "Closing socket ... " << std::endl;
    close(client);
    return 0;

}

