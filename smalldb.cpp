#include <arpa/inet.h>
#include <asm-generic/socket.h>
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


#include "utils.hpp"
#include "db.hpp"
#include "common.hpp"

#define PORT (28772)
#define SOCKETERROR (-1)
#define THREAD_POOL_SIZE (20)
#define BUFFSIZE (200)

int sigint = 1;
pthread_mutex_t newAccess;
pthread_mutex_t writeAccess;
pthread_mutex_t readerAccess;
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



int main(int argc, const char* argv[]) {
    newAccess = PTHREAD_MUTEX_INITIALIZER;
    writeAccess = PTHREAD_MUTEX_INITIALIZER;
    readerAccess = PTHREAD_MUTEX_INITIALIZER;
    readerC = 0;
    std::cout << "Hello, I'm the server " << PORT <<  std::endl;

    std::cout << "Loading the Db" << std::endl;
    database_t *db = new database_t();
    db_load(db, argv[argc-1]);

    struct sigaction action;
    action.sa_handler = signalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGINT, &action, NULL);


    int serverSocket = check(socket(AF_INET, SOCK_STREAM, 0), "Serv: failed to create socket");
    int clientSocket =1;
    int opt =1;
    check(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)),
                "Set option failed");
    struct sockaddr_in address, clientAddress;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    //inet_pton(AF_INET, "173.2.0.1", &address.sin_addr);

    check(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)), "Bind failed !");
    check(listen(serverSocket, 10), "Listen failed !");
    size_t addrlenf = sizeof(clientAddress);
    std::vector<pthread_t*> threads;
    std::vector<int> sockets;
    //cout la size et prendre à chaque fois le dernier thread
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
            //printf("server: got connection from %s port %d\n",
            //inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
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
    std::cout << "Saving the database before closing" << std::endl;
    db_save(db);
    delete db;
    db = nullptr;
    std::cout << "The server is closing" << std::endl;
    close(serverSocket);
    std::cout << "closed" << std::endl;
    return 0;
}
