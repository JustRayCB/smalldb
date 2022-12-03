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

void printResult(const Client &client, const std::vector<std::string> results){
        //std::string size = std::to_string(results.size()-1);
        //send(client.socket, size.c_str(), size.size(), 0);
        int valueSize;
        int convertedSize;
        //std::string res = std::accumulate(results.begin(), results.end(), std::string{});
        std::string res;
        for (auto &value : results) {
            res +=value;
            std::cout << value;
        }
        valueSize = res.length();
        convertedSize = htonl(valueSize);
        std::cout << valueSize << std::endl;
        send(client.socket, &convertedSize, sizeof(convertedSize), 0);
        send(client.socket, res.c_str(), res.size(), 0);
        //for (auto &value : results) {
            ////send(client.socket, value.c_str(), value.size(), 0);
            //std::cout << value;
            //valueSize = value.length();
            //convertedSize = htonl(valueSize);
            //std::cout << valueSize << std::endl;
            //send(client.socket, &convertedSize, sizeof(convertedSize), 0);
        //}

}

void *handleConnection(void *pClient){
    Client client = *static_cast<Client *>(pClient);
    delete static_cast<Client *>(pClient);

    char msg[BUFFSIZE];
    std::string tmp;
    size_t bytesRead;
    std::vector<std::string> results;

    while ((bytesRead = recv(client.socket, msg, BUFFSIZE, 0)) > 0) {
        std::cout <<"Server has received : " << msg << std::endl;
        tmp = msg;
        // check message
        if (tmp.substr(0, 6) == "select"){
            results = select(client.db, tmp.substr(7, tmp.length()));
        }
        // else  if (msg.substr(0, 6) == "insert"{
        // call insert
        //}
        // else if (msg.substr(0, 6) == "update"{
        // call update
        //}
        // else if (msg.substr(0, 6) == "delete"{
        // call delete
        //}
        // else{
        // std::cout << "Unknown method" << std::endl;
        //}
        printResult(client, results);
        memset(msg, 0, BUFFSIZE);
        
    }
    std::cout << "The Client (" << client.socket << ") has been disconnected" << std::endl;
    check(bytesRead, "recv error");
    close(client.socket);

    pthread_exit(nullptr);

    return nullptr;
}

void signalHandler(int signum) {
   if (signum == SIGINT) { //^C
                           //Save and end programm
        //std::cout << "Handling SIGINT signal ... "<< std::endl;
        //fclose(stdin); // Close stdin to get out of the waiting for input
    }
}


int check(int exp, const char *msg){
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}
