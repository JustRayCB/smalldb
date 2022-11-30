#include <cstddef>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "utils.hpp"

#define PORT (28772)
#define SOCKETERROR (-1)
#define THREAD_POOL_SIZE (20)
#define BUFFSIZE (200)



void *handleConnection(void *pClient){
    int client = *static_cast<int *>(pClient);
    delete static_cast<int *>(pClient);

    char msg[BUFFSIZE];
    size_t bytesRead;

    while ((bytesRead = recv(client, msg, BUFFSIZE, 0)) > 0) {
        std::cout <<"Server has received : " << msg << std::endl;
        
    }
    std::cout << "The Client (" << client << ") has been disconnected" << std::endl;
    check(bytesRead, "recv error");
    close(client);


    return nullptr;
}

int main() {
    std::cout << "Hello, I'm the server" << PORT <<  std::endl;

    int serverSocket = check(socket(AF_INET, SOCK_STREAM, 0), "Serv: failed to create socket");
    int clientSocket =1;
    struct sockaddr_in address, clientAddress;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    check(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)), "Bind failed !");
    check(listen(serverSocket, 10), "Listen failed !");
    size_t addrlenf = sizeof(clientAddress);
    std::vector<pthread_t> threads(1);
    //cout la size et prendre à chaque fois le dernier thread

    while (clientSocket != -1 and std::cin) {
        std::cout << "Waiting for connections ... " << std::endl;
        check(clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, 
                    (socklen_t *)&addrlenf), "Accept failed");
        int *pClient = new int(clientSocket);
        std::cout << "Client (" << clientSocket <<") connected" << std::endl;
        pthread_t thread;
        pthread_create(&thread, nullptr, handleConnection, pClient);
    
    }
    std::cout << "The server is closing" << std::endl;
    close(serverSocket);

    return 0;
}
