#ifndef _QUERY_HPP
#define _QUERY_HPP

#include <tuple>
#include <vector>
#include <string>

#include "db.hpp"


std::string *select(database_t *database, std::string query);
std::string *insert(database_t *database, std::string query);
std::string *deletion(database_t *database, std::string query);
std::string *update(database_t *database, std::string query);
std::tuple<bool, int> findStudent(database_t *database, std::string &field, std::string &value, std::string &results,
        const std::string &fieldToUpdate = "", const std::string &updateValue = "");


#endif

