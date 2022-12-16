#include <arpa/inet.h>
#include <cstddef>
#include <cstring>
#include <semaphore.h>
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


#include "utils.hpp"
#include "db.hpp"
#include "common.hpp"

#define PORT (28772)
#define SOCKETERROR (-1)
#define THREAD_POOL_SIZE (20)
#define BUFFSIZE (200)

int sigint = 1;
sem_t newAccess;
sem_t writeAccess;
sem_t readerAccess;
int readerC;

void signalHandler(int signum) {
    if (signum == SIGINT) {
        std::cout << std::endl << "Handling SIGINT ..." << std::endl;
        //fclose(stdin);
        sigint = 0;
    } else if (signum == SIGUSR1) {
        std::cout << "Handling SIGUSR1 ..." << std::endl;
        sigint = 2;
    }
}


void configSocket(int &serverSocket, struct sockaddr_in &address){
    //struct sockaddr_in address, clientAddress;
    serverSocket = check(socket(AF_INET, SOCK_STREAM, 0), "Serv: failed to create socket");
    int opt=1;
    check(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)),
                "Set option failed");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    check(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)), "Bind failed !");
    check(listen(serverSocket, 10), "Listen failed !");
    
}

int main(int argc, const char* argv[]) {
    sem_init(&newAccess, 0, 1);
    sem_init(&writeAccess, 0, 1);
    sem_init(&readerAccess, 0, 1);
    readerC = 0;
    std::cout << "Loading the Db" << std::endl;
    database_t *db = new database_t();
    db_load(db, argv[argc-1]);

    struct sigaction action;
    action.sa_handler = signalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGINT, &action, NULL);


    struct sockaddr_in address, clientAddress;
    int serverSocket; int clientSocket;
    configSocket(serverSocket, address);

    size_t addrlenf = sizeof(clientAddress);
    std::vector<pthread_t*> threads; std::vector<int> sockets;
    while (sigint) {
        if (sigint == 2) {
                std::cout << "Saving the database" << std::endl;
                db_save(db);
                sigint = 1;
        }
        std::cout << "Waiting for connections ... " << std::endl;
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress,
                    (socklen_t *)&addrlenf);

        if (clientSocket < 0) {
            if (errno == EINTR) {
                std::cout << "Catch SIGNAL" << std::endl;
                continue;
            }
        }
        else {
            //Bloque le signal pour le thread courant
            printf("server: got connection from %s port %d\n",
            inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            sigaddset(&mask, SIGINT);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            Client *pClient = new Client({clientSocket, db});
            std::cout << "Client (" << clientSocket <<") connected" << std::endl;
            pthread_t *thread = new pthread_t();
            pthread_create(thread, nullptr, handleConnection, pClient);
            threads.push_back(thread);
            sockets.push_back(clientSocket);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }

    }
    std::cout << "The server is closing" << std::endl;
    close(serverSocket);
    std::cout << "closed" << std::endl;
    if(system("/bin/bash -c ./killClient")){
        std::cout << "Problem when killing client ! " << std::endl;
    }
    for (auto &current : threads) {
        pthread_cancel(*current);
        pthread_join(*current, nullptr);
        delete current;
    }
    for (auto &socket : sockets) {
        close(socket);
    }
    std::cout << "Saving the database before closing the program" << std::endl;
    db_save(db);
    delete db;
    db = nullptr;
    return 0;
}
