#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

#include "db.hpp"
#include "query.hpp"
#include "parsing.hpp"
#include "student.hpp"


void updateStudent(student_t &student, const std::string &fieldToUpdate, const std::string &updateValue){
  if (fieldToUpdate == "id") {
    student.id = (unsigned)stoi(updateValue);
  }else if (fieldToUpdate == "fname") {
    strcpy(student.fname, updateValue.c_str());
  }else if (fieldToUpdate == "lname") {
    strcpy(student.lname, updateValue.c_str());
  }else if (fieldToUpdate == "section") {
    strcpy(student.section, updateValue.c_str());
  }else {
    int day, mon, year;
    parse_birthdate(updateValue, day, mon, year);
    student.birthdate.tm_mday = day;
    student.birthdate.tm_mon = mon;
    student.birthdate.tm_year = year;
  }


}

bool isSearchedStudent(const std::string &field, const std::string &value, const student_t &student){
    if (field == "id") {
        if (std::to_string(student.id) == value) {
            return true;
        }
    }
    else if (field == "fname") {
        if (student.fname == value) {
            return true;
        }
    }

    else if (field == "lname") {
        if (student.lname == value) {
            return true;
        }
    }

    else if (field == "section") {
        if (student.section == value) {
            return true;
        }
    }
    else if (field == "birthdate") {
        int day, mon, year;
        if (parse_birthdate(value, day, mon, year)) {
            if ((day == student.birthdate.tm_mday) and
                (mon == student.birthdate.tm_mon) and
                (year == student.birthdate.tm_year)) {
                return true;
            }
        }else {
            std::cout << "Problem with the birthdate" << std::endl;
            return false;
        }
    }

    return false;
}

bool findStudent(database_t *database, std::string &field, std::string &value, 
        std::vector<std::string> &results,
        const std::string &fieldToUpdate, const std::string &updateValue){
    bool correctField = false;
    bool isUpdate = false, isDelete = false;
    for (auto &allFields : {"id", "fname", "lname", "section", "birthdate"}) {
        if (field == allFields) {
            correctField = true;
            break;
        }
    
    }

    if (not correctField) {
        results.push_back("Problem with the query field");
        return false;
    }

    if (fieldToUpdate != "" and fieldToUpdate != "delete") {
        isUpdate = true;
    }
    if (fieldToUpdate == "delete") {
        isDelete = true;

    }
    size_t idx = 0;
    size_t sizeVector = database->data.size();

    while (idx < sizeVector) {
        auto &student = database->data[idx];
        if (isSearchedStudent(field, value, student)) {
            if (isUpdate) {
                updateStudent(student, fieldToUpdate, updateValue);
                results.push_back(student_to_str(student));
            }
            else if (not isUpdate and not isDelete) {
                results.push_back(student_to_str(student));
            }
            else {
                results.push_back(student_to_str(student));
                database->data.erase(database->data.begin()+idx);
                sizeVector--;
                idx--;
            }
        }
        idx++;

    
    }
    //for (auto &student : database->data) {
        //if (isSearchedStudent(field, value, student)) {
            //if (isUpdate) {//delete function
                //updateStudent(student, fieldToUpdate, updateValue);
                //results.push_back(student_to_str(student));
            //}
            //else if (not isUpdate and not isDelete) { //select function
                //results.push_back(student_to_str(student));
            //}
            //else {
                //results.push_back(student_to_str(student));
                ////for (auto stdu : database->data) {
                    ////std::cout << "here" << std::endl;
                    ////std::cout << student_to_str(stdu);
                ////}
                ////std::cout << database->data.begin() << std::endl;
                ////std::cout << student_to_str(database->data[9]); 
                ////database->data.erase(database->data.begin()+idx);
            //}
            //if (field == "id") {
                //return true;
            //}
        //}
        //idx++;
    //}
    return true;
}

std::vector<std::string> select(database_t *database, std::string query){
    //std::string *results = new std::string();
    std::vector<std::string> results;
    std::string field, value;

    if ( not parse_selectors(query, field, value)){
        results.push_back("Problem with the query select \n"
                    "Please enter the arguments correctly \n");
        return results;
    }
    
    bool ret = findStudent(database, field, value, results);
    if (not ret) {
        results.push_back("Problem with the query select \n"
                    "There is a problem that was not suppose to happened\n");
        return results;
    }
    results.push_back(std::to_string(results.size())+" student(s) selected" + '\n');
    return results;
}

std::vector<std::string> update(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string fieldFilter, valueFilter, fieldToUpdate, updateValue;

    if (not parse_update(query, fieldFilter, valueFilter, fieldToUpdate, updateValue)) {
        results.push_back("Problem with the query update \n"
                    "Please enter the arguments correctly \n");
        return results;
    }
    
    bool ret = findStudent(database, fieldFilter, valueFilter, results, fieldToUpdate, updateValue);
    if (not ret) {
        results.push_back("Problem with the query update \n"
                    "There is a problem that was not suppose to happened\n");
        return results;
    }

    return {std::to_string(results.size())+" student(s) updated" + '\n'};
}

std::vector<std::string> insert(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string fname, lname, id, section, birthdate;
    if (not parse_insert(query, fname, lname, id, section, birthdate)) {
        results.push_back("Problem with the query insert \n"
                    "Please enter the arguments correctly \n");
        return results;
    }
    student_t student;
    updateStudent(student, "fname", fname);
    updateStudent(student, "lname", lname);
    updateStudent(student, "id", id);
    updateStudent(student, "section", section);
    updateStudent(student, "birthdate", birthdate);
    for (auto &currentStudent : database->data) {
        if (currentStudent.id == student.id) {
            results.push_back("Error id is already in the database\n");
            return results;
        }
    }
    results.push_back(student_to_str(student));
    db_add(database, student);
    return results;

}

std::vector<std::string> deletion(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string field, value;
    if (not parse_selectors(query, field, value)) {
        results.push_back("Problem with the query insert \n"
                    "Please enter the arguments correctly \n");
        return results;
    }

    bool ret = findStudent(database, field, value, results, "delete");
    if (not ret) {
        results.push_back("Problem with the query update \n"
                    "There is a problem that was not suppose to happened\n");
        return results;
    }

    return {std::to_string(results.size())+" student(s) deleted" + '\n'};
}
