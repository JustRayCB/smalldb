#ifndef _PARSING_H
#define _PARSING_H

#include <string>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* Required to work with strptime, which is OS-dependent */
#endif

#include <stdbool.h>

#include "student.hpp"

/*
 * parsing.hpp file for the program tinyDb
 * Groupe: Callens Hugo:000513303, Contuliano Bravo Rayan:000537050, Zaleski Arkadiusz:000516050 
 */




/** 
 * Parse the arguments of an update query.
 * Example:
 * ```
 * char* query = "update id=864030 set fname=Trucmuche"
 * char *command = strtok_r(query, " ", &query);
 * parse_update(query, field_filter, value_filter, field_to_update, update_value);
 * ```
 * @return: true if the parsing was successful, false otherwise
 **/
bool parse_update(std::string &query, std::string& field_filter, std::string &value_filter, std::string& field_to_update,
        std::string &update_value);

/**
 * Parse the arguments of an insert query.
 * An insert query must specify all the fields as follows:
 * ```
 * char* query = "insert Yannick Molinghen 393354 info 14/02/1995"
 * char* command = strtok_r(query, " ", &query);
 * parse_insert(query, fname, lname, &id, section, &birthdate);
 * ```
 * @return: true if the parsing was successful, false otherwise
 **/
bool parse_insert(std::string& query_arguments, std::string& fname, std::string& lname, std::string& id, std::string& section, std::string& birthdate);

/** 
 * Parse field=value selectors.
 **/
bool parse_selectors(std::string &query, std::string &field, std::string &value);

/*
 * @brief: Parse a birthdate value and update all arguments with the integer
 * value
 *
 * @return If the parsing was successful, false otherwise
 */
bool parse_birthdate(std::string &query, int &day, int &mon, int &year);

#endif
