#include <cstdio>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define PORT (28772)
#define SOCKETERROR (-1)
#define BUFFSIZE (200)

int check(int exp, const char *msg){
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}



int main(){
    
    int client = check(socket(AF_INET, SOCK_STREAM, 0), "Client: failed to create socket");
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        
    check(connect(client, (struct sockaddr *) &serv_addr, sizeof(serv_addr)), "Connection failed");
    
    std::string msg;

    while (std::getline(std::cin, msg)){
      std::cout << "Client sending to server.." << std::endl;
      std::cout << "Client msg -> " << msg << std::endl;
      send(client, msg.c_str(), msg.size(), 0);
    }
    close(client);
    return 0;

}

