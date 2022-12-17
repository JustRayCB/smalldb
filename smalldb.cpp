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

/*
 * Main file for the server side
 * Groupe: Contuliano Bravo Rayan:000537050, Callens Hugo:000513303, Zaleski Arkadiusz:000516050
 */

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

/*
 * @brief: Function that will config the server socket
 */
void configSocket(int &serverSocket, struct sockaddr_in &address){
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

/*@brief: Function that will manage signals for new created threads
 * and stock it to a vector same for the client socket
 */
void handleNewThread(int &clientSocket, std::vector<pthread_t*> &allThreads,
        std::vector<int> &allClients, database_t *db){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1); sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    Client *pClient = new Client({clientSocket, db});
    std::cout << "Client (" << clientSocket <<") connected" << std::endl;
    pthread_t *thread = new pthread_t();
    pthread_create(thread, nullptr, handleConnection, pClient);
    allThreads.push_back(thread);
    allClients.push_back(clientSocket);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

}

/*
 * @brief Function that kill all client and wait for their thread to finish then
 * close all sockets
 */
void terminateServerAndClient(int &serverSocket, std::vector<pthread_t*> allThreads, std::vector<int> allClients){
    std::cout << "The server is closing" << std::endl;
    close(serverSocket);
    std::cout << "closed" << std::endl;
    if(system("/bin/bash -c ./killClient")){ //Call a bash program that send SIGINT to all client to close them correctly
        std::cout << "Problem when killing client ! " << std::endl;
    }
    for (auto &current : allThreads) {
        pthread_cancel(*current);
        pthread_join(*current, nullptr);
        delete current;
    }
    for (auto &socket : allClients) {
        close(socket);
    }
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
    sigaction(SIGUSR1, &action, NULL); sigaction(SIGINT, &action, NULL);


    struct sockaddr_in address, clientAddress;
    int serverSocket; int clientSocket;
    configSocket(serverSocket, address);

    size_t addrlenf = sizeof(clientAddress);
    std::vector<pthread_t*> allThreads; std::vector<int> allClients;
    while (sigint) {
        if (sigint == 2) {
                db_save(db); sigint = 1;
        }
        std::cout << "Waiting for connections ... " << std::endl;
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress,
                    (socklen_t *)&addrlenf);
        if (clientSocket > 0) {
            handleNewThread(clientSocket, allThreads, allClients, db);
        }

    }
    terminateServerAndClient(serverSocket, allThreads, allClients);
    
    std::cout << "Saving the database before closing the program" << std::endl;
    db_save(db); delete db; db = nullptr;
    return 0;
}
