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

#define PORT (28772)
#define SOCKETERROR (-1)
#define THREAD_POOL_SIZE (20)
#define BUFFSIZE (200)


void signalHandler(int signum) {
   if (signum == SIGINT) { //^C
                           //Save and end programm
       std::cout << "Handling SIGINT signal ... "<< std::endl;
     fclose(stdin); // Close stdin to get out of the waiting for input
    }
}


int check(int exp, const char *msg){
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}

void *handleConnection(void *pClientSocket){
    int client = *static_cast<int *>(pClientSocket);
    delete static_cast<int *>(pClientSocket);

    char msg[BUFFSIZE];
    size_t bytesRead;

    //std::cout << "hehee" << std::endl;
    //std::cout <<(bytesRead = recv(client, msg, BUFFSIZE, 0)) << std::endl;
    while ((bytesRead = recv(client, msg, BUFFSIZE, 0)) > 0) {
        std::cout <<"Server has received : " << msg << std::endl;
        std::cout << "last : " << msg[BUFFSIZE -1] << std::endl;
        std::cout << bytesRead << std::endl;
        if (bytesRead > BUFFSIZE and msg[BUFFSIZE-1] != '\n') {
            std::cout << "here" << std::endl;
            bytesRead = -1;
            //check(bytesRead, "Size too much");
            break;
        }
        
    }
    std::cout << "hehee" << std::endl;
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
        std::cout << "Client connected" << std::endl;
        int *pClient = new int(clientSocket);
        pthread_t thread;
        pthread_create(&thread, nullptr, handleConnection, pClient);
    
    }
    std::cout << "The server is closing" << std::endl;
    close(serverSocket);

    return 0;
}
