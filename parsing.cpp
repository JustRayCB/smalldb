#include "parsing.hpp"

#include <exception>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <time.h>

#include "student.hpp"

using namespace std;


bool parse_update(string &query, string& field_filter, string &value_filter, string& field_to_update,
        string &update_value){

    stringstream X(query);

    string key_val_filter;
    if (!getline(X, key_val_filter, ' ')) {
        return false;
    }
    string tmp;
    if (!getline(X, tmp, ' ')) {
        return false;
    }
    string key_val_update;
    if (!getline(X, key_val_update, ' ')) {
        return false;
    }

    if (!parse_selectors(key_val_filter, field_filter, value_filter)) {
        return false;
    }

    if (!parse_selectors(key_val_update, field_to_update, update_value)) {
        return false;
    }
    return true;

}



bool parse_insert(std::string& query_arguments, std::string& fname, std::string& lname, std::string& id, std::string& section, std::string& birthdate){
    stringstream X(query_arguments);
    string token;

    if (!getline(X, token, ' ')) {
        return false;
    }
    
    fname = token;

    if (!getline(X, token, ' ')) {
        return false;
    }

    lname = token;
    if (!getline(X, token, ' ')) {
        return false;
    }

    id = token;
    unsigned tmp;
    try {
    tmp = (unsigned)stoi(id);
    } catch (const invalid_argument& ia) {
        std::cerr << "Invalid argument: " << ia.what() << std::endl;
        return false;
    } catch (const exception& e){
        std::cerr << "Unknown error : " << e.what() << std::endl;
        return false;
    }
    tmp++; // Just not to have the W-Unused warning

    if (!getline(X, token, ' ')) {
        return false;
    }

    section = token;

    if (!getline(X, token, ' ')) {
        return false;
    }

    birthdate = token;

    int day, mon, year;
    if (!parse_birthdate(birthdate, day, mon, year)) {
        return false;
    }

    return true;

}

bool parse_selectors(string &query, string &field, string &value){
    stringstream X(query);

    string token;

    if (!getline(X, token, '=')){
        return false;
    }
    field = token;
    if (!getline(X, token, '=')){
        return false;
    }
 
    value = token;

    if (field == "id") { // If the field is Id check if its an integer
         unsigned tmp;
        try {
        tmp = (unsigned)stoi(value);
        } catch (const invalid_argument& ia) {
            std::cerr << "Invalid argument: " << ia.what() << std::endl;
            return false;
        } catch (const exception& e){
            std::cerr << "Unknown error : " << e.what() << std::endl;
            return false;
        }
        tmp++; // Just not to have the W-Unused warning
   }

    if (field == "birthdate") {
        int day, mon, year;
        if (!parse_birthdate(value, day, mon, year)) {
            return false;
        }
    }
    return true;
} 

bool parse_birthdate(string &query, int &day, int &mon, int &year){
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
