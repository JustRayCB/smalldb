#ifndef _UTILS_HPP
#define _UTILS_HPP

#include "db.hpp"

struct Client{
    const int socket;
    database_t *db;
};


int check(int exp, const char *msg);

/*
 * @brief: Main function for a thread
 *
 * @param client: Client struct with the client socket and a link to the database
 */
void *handleConnection(void *client);

/*
 * @brief: Function that will handle the client's command by calling the correct
 * method
 * @param results: ref to the vector of string that stock all the query results
 * @param bytesRead: ref to integer that stock the bytes read by recv
 * @param client: ref to the client socket and db
 * @return 0 if the went good 1 if there were a problem
 */
int handleCommand(std::vector<std::string> &results, size_t &bytesRead, Client &client);

/*
 */
int printResult(const Client &client, std::vector<std::string> &results);
#endif
