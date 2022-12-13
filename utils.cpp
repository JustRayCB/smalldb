#include <cstddef>
#include <cstdint>
#include <iostream>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <numeric>

#include "utils.hpp"
#include "query.hpp"

#define SOCKETERROR (-1)
#define BUFFSIZE (200)

void printResult(const Client &client, std::vector<std::string> &results){
        int valueSize;
        int convertedSize;
        valueSize = results.size();
        convertedSize = htonl(valueSize);
        //std::cout << "size : " << valueSize << std::endl;
        send(client.socket, &convertedSize, sizeof(convertedSize), 0);
        //check(write(client.socket, &convertedSize, sizeof(convertedSize)), "Write failed");
        size_t totalSize = 0;
        char ok[3];
        for (auto &value : results) {
            //std::cout << value;
            value += '\0';
            valueSize = value.length();
            convertedSize = htons(valueSize);
            send(client.socket, &convertedSize, sizeof(convertedSize), 0);
            send(client.socket, value.data(), valueSize, 0);
            recv(client.socket, ok, 3, 0); //block
            totalSize += valueSize;
        }
        //std::cout << "The bytes I sent : " << totalSize << std::endl;
}

void *handleConnection(void *pClient){
    Client client = *static_cast<Client *>(pClient);
    delete static_cast<Client *>(pClient);

    char msg[BUFFSIZE];
    std::string tmp;
    size_t bytesRead;
    std::vector<std::string> results;

    while ((bytesRead = recv(client.socket, msg, BUFFSIZE, 0)) > 0) {
        //std::cout <<"Server has received : " << msg << std::endl;
        tmp = msg;
        // check message
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
         //std::cout << "Unknown method" << std::endl;
         results = {"Unknown query type\n"};
        }
        printResult(client, results);
        memset(msg, 0, BUFFSIZE);
        
    }
    std::cout << "The Client (" << client.socket << ") has been disconnected" << std::endl;
    check(bytesRead, "recv error");
    close(client.socket);

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
