#include <iostream>
#include <vector>
#include <string>

#include "query.hpp"
#include "parsing.hpp"
#include "student.hpp"



bool findStudent(database_t *database, std::string &field, std::string &value, std::vector<std::string> &results){
    for (auto &student : database->data) {
        if (field == "id") {
            if (std::to_string(student.id) == value) {
                results.push_back(student_to_str(student));
                //results + student_to_str(student);
                return true;
            }
        }
        else if (field == "fname") {
            if (student.fname == value) {
                //results + student_to_str(student);
                results.push_back(student_to_str(student));
            }
        }

        else if (field == "lname") {
            if (student.lname == value) {
                //results + student_to_str(student);
                results.push_back(student_to_str(student));
            }
        }

        else if (field == "section") {
            if (student.section == value) {
                //results + student_to_str(student);
                results.push_back(student_to_str(student));
            }
        } 
        else if (field == "birthdate") {
            int day, mon, year;
            if (parse_birthdate(value, day, mon, year)) {
                if ((day == student.birthdate.tm_mday) and 
                    (mon == student.birthdate.tm_mon) and
                    (year == student.birthdate.tm_year)) {
                        //results + student_to_str(student);
                        results.push_back(student_to_str(student));
                }
            }else {
                std::cout << "Problem with the birthdate" << std::endl;
                return false;
            }
        }else {
            std::cout << "Problem with the query field" << std::endl;
            return false;
        }
    }
    return true;
}

std::vector<std::string> select(database_t *database, std::string query){
    std::vector<std::string> results;
    std::cout << "size of vector : " << results.size() << std::endl;
    std::string field, value;

    if (!parse_selectors(query, field, value)){
        std::cout << "Problem with the query select" << std::endl;
        std::cout << "Please enter the arguments correctly" << std::endl;
        return results;
    }
    
    bool ret = findStudent(database, field, value, results);
    if (not ret) {
        std::cout << "Problem with the query select" << std::endl;
        return results;
    }
    //results + std::to_string(results.size())+" student(s) selected" + '\n';
    results.push_back(std::to_string(results.size())+" student(s) selected" + '\n');
    return results;
}
