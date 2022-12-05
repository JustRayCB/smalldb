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

std::tuple<bool, int> findStudent(database_t *database, std::string &field, std::string &value, std::string &results,
        const std::string &fieldToUpdate, const std::string &updateValue){
    bool correctField = false;
    bool isUpdate = false;
    int studentFound = 0;
    for (auto &allFields : {"id", "fname", "lname", "section", "birthdate"}) {
        if (field == allFields) {
            correctField = true;
            break;
        }
    
    }
    if (fieldToUpdate != "") {
        correctField = true;
    }
    if (not correctField) {
        std::cout << "Problem with the query's field" << std::endl;
        return {false, studentFound};
    }
    for (auto &student : database->data) {
        if (isSearchedStudent(field, value, student)) {
            studentFound++;
            if (isUpdate) {
                updateStudent(student, fieldToUpdate, updateValue);
            }
            //results.push_back(student_to_str(student));
            results+= student_to_str(student);
            //if (isDelete) {
                //deleteStudent(student);
            //}
            if (field == "id") {
                return {true, studentFound};
            }
        }
    }
    return {true, studentFound};
}

std::string *select(database_t *database, std::string query){
    std::string *results = new std::string();
    std::string field, value;

    if ( not parse_selectors(query, field, value)){
        *results +=  "Problem with the query select \n"
                    "Please enter the arguments correctly \n";
        return results;
    }
    
    std::tuple<bool, int> ret = findStudent(database, field, value, *results);
    if (not std::get<0>(ret)) {
        *results += "Problem with the query select \n"
                    "There is a problem that was not suppose to happened\n";
        return results;
    }
    *results += std::to_string(std::get<1>(ret)) +" student(s) selected" + '\n';
    //results.push_back(std::to_string(results.size())+" student(s) selected" + '\n');
    return results;
}

std::string *update(database_t *database, std::string query){
    std::string *results = new std::string();
    std::string fieldFilter, valueFilter, fieldToUpdate, updateValue;

    if (not parse_update(query, fieldFilter, valueFilter, fieldToUpdate, updateValue)) {
        *results +=  "Problem with the query update \n"
                    "Please enter the arguments correctly \n";
        return results;
    }
    
    std::tuple<bool, int> ret = findStudent(database, fieldFilter, valueFilter, *results, fieldToUpdate, updateValue);
    if (not std::get<0>(ret)) {
        *results += "Problem with the query update \n"
                    "There is a problem that was not suppose to happened\n";
        return results;
    }

    *results += std::to_string(std::get<1>(ret)) +" student(s) updated" + '\n';
    
    return results;
}

std::string *insert(database_t *database, std::string query){
    std::string *results = new std::string();
    std::string fname, lname, id, section, birthdate;
    if (not parse_insert(query, fname, lname, id, section, birthdate)) {
        *results +=  "Problem with the query update \n"
                    "Please enter the arguments correctly \n";
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
            *results += "Error id is already in the database\n";
            return results;
        }
    }
    //*results += std::to_string(std::get<1>(ret)) +" student(s) selected" + '\n';
    *results += student_to_str(student);
    db_add(database, student);
    return results;

}
