#include <cstddef>
#include <cstdint>
#include <iostream>
#include <signal.h>
#include <string>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <numeric>
#include <poll.h>

#include "utils.hpp"
#include "query.hpp"

#define SOCKETERROR (-1)
#define BUFFSIZE (200)


int printResult(const Client &client, std::vector<std::string> &results){
        int valueSize;int convertedSize;
        valueSize = results.size();
        convertedSize = htonl(valueSize);
        if(send(client.socket, &convertedSize, sizeof(convertedSize), 0)<0){return 1;};
        char ok[3];
        for (auto &value : results) {
            value += '\0';
            valueSize = value.length();
            convertedSize = htons(valueSize);
            if(send(client.socket, &convertedSize, sizeof(convertedSize), 0)<0){return 1;}
            if(send(client.socket, value.data(), valueSize, 0) < 0){return 1;}
            if(recv(client.socket, ok, 3, 0) < 0){return 1;}
        }
        return 0;
}

int handleCommand(std::vector<std::string> &results, size_t &bytesRead, Client &client){
    char msg[BUFFSIZE];
    std::string tmp;
    bytesRead = recv(client.socket, msg, BUFFSIZE, 0);
    if (bytesRead == static_cast<size_t>(-1)) {
        bytesRead = -1;
        return 1;
    }else if (not bytesRead) {
        return 1;
    }
    tmp = msg;
    if (tmp.size() <= 6) {
        tmp+= " "; //if the size is <=6 and = to a real query, there will be a OutOfRange error
    }
    if (tmp.substr(0, 6) == "select"){
        results = select(client.db, tmp.substr(7, tmp.length()));
    }
    else  if (tmp.substr(0, 6) == "insert"){
        results = insert(client.db, tmp.substr(7, tmp.length()));
    }
    else if (tmp.substr(0, 6) == "update"){
        results = update(client.db, tmp.substr(7, tmp.length()));
    }
    else if (tmp.substr(0, 6) == "delete"){
        results = deletion(client.db, tmp.substr(7, tmp.length()));
    }
    else{
     results = {"Unknown query type\n", "Please enter one of this commands"
     " select, insert, update or delete\n"};
    }
    return 0;

}

void *handleConnection(void *pClient){
    Client client = *static_cast<Client *>(pClient);
    delete static_cast<Client *>(pClient);

    size_t bytesRead = 0;
    std::vector<std::string> results;

    struct pollfd fdClient[1];
    fdClient[0].fd = client.socket;
    fdClient[0].events = POLLIN; 

    while (true) {
     int isTimerRespected = poll(
         fdClient, 1, 25000); // Set a timer of 25 sec, if it's not respected
                              //The client is considered by disconnected
        if (isTimerRespected < 0) {
            bytesRead = 1; //Unknown error
            break;
        }else if (not isTimerRespected) {
            bytesRead = -2; //the client is disconnected normally
            break;
        }else {
            if (handleCommand(results, bytesRead, client)) {break;}
            if(printResult(client, results)){break; std::cout << "Probleme while sending/receiving data" << std::endl;}
        }
    }
    if (bytesRead == 0) {
        std::cout << "The Client (" << client.socket << ") has been disconnected" << std::endl;
    }
    else if (bytesRead == static_cast<size_t>(-1)) {
        std::cout << "Lost connection with the Client (" << client.socket << ") " << std::endl;
    }
    else if (bytesRead == static_cast<size_t>(-2)) {
        std::cout << "The Client (" << client.socket << ") has been disconnected due "
            "to innactivity" << std::endl;
    }else {
        std::cout << "Unknown error with the Client (" << client.socket << ") " << std::endl;
    }
    close(client.socket);
    std::cout << "Closing the thread of Client (" << client.socket << ") " << std::endl;
    pthread_exit(nullptr);
    return nullptr;
}



int check(int exp, const char *msg){
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}
