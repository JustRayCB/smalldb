#include <cstddef>
#include <cstring>
#include <string>
#include <sys/select.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>


#include "student.hpp"
#include "utils.hpp"
#include "db.hpp"
#include "query.hpp"

#define PORT (28772)
#define SOCKETERROR (-1)
#define THREAD_POOL_SIZE (20)
#define BUFFSIZE (200)




int main(int argc, const char* argv[]) {
    std::cout << "Hello, I'm the server " << PORT <<  std::endl;

    std::cout << "Loading the Db" << std::endl;
    database_t *db = new database_t();
    db_load(db, argv[argc-1]);


    int serverSocket = check(socket(AF_INET, SOCK_STREAM, 0), "Serv: failed to create socket");
    int clientSocket =1;
    struct sockaddr_in address, clientAddress;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    check(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)), "Bind failed !");
    check(listen(serverSocket, 10), "Listen failed !");
    size_t addrlenf = sizeof(clientAddress);
    std::vector<pthread_t> threads;
    //cout la size et prendre à chaque fois le dernier thread

    while (clientSocket != -1 and std::cin) {
        std::cout << "Waiting for connections ... " << std::endl;
        check(clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, 
                    (socklen_t *)&addrlenf), "Accept failed");
        //int *pClient = new int(clientSocket);
        Client *pClient = new Client({clientSocket, db});
        std::cout << "Client (" << clientSocket <<") connected" << std::endl;
        pthread_t thread;
        pthread_create(&thread, nullptr, handleConnection, pClient);
        threads.push_back(thread);
    
    }
    for (auto &current : threads) {
        pthread_join(current, nullptr);
    }
    delete db;
    db = nullptr;
    std::cout << "The server is closing" << std::endl;
    close(serverSocket);

    return 0;
}
