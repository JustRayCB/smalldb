#include "parsing.hpp"

#include <exception>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <time.h>
#include <vector>

#include "student.hpp"

using namespace std;


void errorParseUpdate(int returnValue, std::vector<std::string> &results){
    if (returnValue == 1) {
        results.push_back("Error: Some fields are missings\n"
                "Make sure you used the correct format\n"
                "--> update <Filter>=<value> set <Filter we want to update>=<Value to update>\n");
    }
    else {
        errorParseSelectors(returnValue, results);
    }


}
int parse_update(string &query, string& field_filter, string &value_filter, string& field_to_update,
        string &update_value){
    stringstream X(query);
    string key_val_filter;

    if (!getline(X, key_val_filter, ' ')) {
        return 1;
    }
    string tmp;
    if (!getline(X, tmp, ' ')) {
        return 1;
    }
    string key_val_update;
    if (!getline(X, key_val_update, ' ')) {
        return 1;
    }

    int retParseSelect = parse_selectors(key_val_filter, field_filter, value_filter);
    if (retParseSelect) {
        return retParseSelect;
    }
    retParseSelect = parse_selectors(key_val_update, field_to_update, update_value);
    if (retParseSelect) {
        return retParseSelect;
    }
    return 0;

}


void errorParseInsert(int returnValue, std::vector<std::string> &results){
    if (returnValue==1) {
        results.push_back("Error: The some fields are missing\n"
                "Make sure you used the correct format\n"
                "--> insert fname lname id section birthdate\n");
    }
    else if (returnValue == 2) {
        results.push_back("Error: Id value is not an integer\n");
    }
    else if (returnValue == 3) {
        results.push_back("Error: Id, Unknown exception\n");
    }
    else if (returnValue == 4) {
        results.push_back("Error: birthdate filter is not in"
                " the right format\n"
                "Make sure it respect -> birthdate=00/00/00\n");
    }

}


int parse_insert(std::string& query_arguments, std::string& fname, std::string& lname, std::string& id, std::string& section, std::string& birthdate){
    stringstream X(query_arguments);
    string token;

    if (!getline(X, token, ' ')) {
        return 1;
    }fname = token;
    if (!getline(X, token, ' ')) {
        return 1;
    }lname = token;
    if (!getline(X, token, ' ')) {
        return 1;
    }id = token;
    unsigned tmp;
    try {
    tmp = (unsigned)stoi(id);
    } catch (const invalid_argument& ia) {
        return 2;
    } catch (const exception& e){
        return 3;
    }tmp++; // Just not to have the W-Unused warning

    if (!getline(X, token, ' ')) {
        return 1;
    }section = token;
    if (!getline(X, token, ' ')) {
        return 1;
    }birthdate = token;

    int day, mon, year;
    if (!parse_birthdate(birthdate, day, mon, year)) {
        return 4;
    }
    return 0;
}

void errorParseSelectors(int returnValue, std::vector<std::string> &results){
    if (returnValue==1) {
        results.push_back("Error: There is a problem with the fields\n"
                "Make sure you used the correct format\n"
                "-->Filter=Value\n");
    }
    else if (returnValue == 2) {
        results.push_back("Error: Id value is not an integer\n");
    }
    else if (returnValue == 3) {
        results.push_back("Error: Id, Unknown exception\n");
    }
    else if (returnValue == 4) {
        results.push_back("Error: birthdate filter is not in"
                " the right format\n"
                "Make sure it respect -> birthdate=00/00/00\n");
    }
}

int parse_selectors(string &query, string &field, string &value){
    stringstream X(query);

    string token;

    if (!getline(X, token, '=')){
        return 1;
    }field = token;
    if (!getline(X, token, '=')){
        return 1;
    }value = token;

    if (field == "id") { // If the field is Id check if its an integer
         unsigned tmp;
        try {
        tmp = (unsigned)stoi(value);
        } catch (const invalid_argument& ia) {
            //std::cerr << "Invalid argument: " << ia.what() << std::endl;
            return 2;
        } catch (const exception& e){
            //std::cerr << "Unknown error : " << e.what() << std::endl;
            return 3;
        }tmp++; // Just not to have the W-Unused warning
   }

    if (field == "birthdate") {
        int day, mon, year;
        if (!parse_birthdate(value, day, mon, year)) {
            return 4;
        }
    }
    return 0;
} 

bool parse_birthdate(string query, int &day, int &mon, int &year){
    stringstream X(query);

    string token;

    if (!getline(X, token, '/')){
        return false;
    }
    try {
        day = stoul(token, nullptr, 10);
    } catch (const invalid_argument &ia) {
        return false;
    }

    if (!getline(X, token, '/')){
        return false;
    }

    try {
        mon = stoul(token, nullptr, 10) -1; // Parce que c'est décalé de 1 dans tm_mon
    } catch (const invalid_argument &ia) {
        return false;
    }

    if (!getline(X, token, '/')){
        return false;
    }
    try {
        year = stoul(token, nullptr, 10)-1900;
    } catch (const invalid_argument &ia) {
        return false;
    }

    return true;


}
