#include <cstdio>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <netinet/in.h>
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
    int valueSize;

    while (std::getline(std::cin, msg)){
        std::cout << "Client sending to server.." << std::endl;
        std::cout << "Client msg -> " << msg << std::endl;
        send(client, msg.c_str(), msg.size(), 0);
        recv(client, &valueSize, sizeof(valueSize), 0);
        valueSize = ntohl(valueSize);
        char *results = new char[valueSize+1];
        results[valueSize] = '\0';
        //char results[84];
        recv(client, results, valueSize, 0);
        //std::string test(results);
        //std::cout <<  test;
        //for (auto idx=0; idx < valueSize; idx++) {
            //std::cout << "idx : " << idx << " : " << results[idx] <<std::endl;
        //}
        std::cout << results;
        //std::cout << results[83] << std::endl;
        //std::cout << results[84] << std::endl;
        delete [] results;
      
    }
    close(client);
    return 0;

}

