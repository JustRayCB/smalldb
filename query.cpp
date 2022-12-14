#include <algorithm>
#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <string>
#include <tuple>

#include "db.hpp"
#include "query.hpp"
#include "parsing.hpp"
#include "student.hpp"
#include "common.hpp"


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

void firstPartMutexReader(){
    if (sem_wait(&newAccess)!= 0) {
        std::cout << "Error while locking the mutex access" << std::endl;
    
    }
    if (sem_wait(&readerAccess)!= 0) {
        std::cout << "Error while locking the mutex reader" << std::endl;
    
    }
    if (readerC == 0) {
        if (sem_wait(&writeAccess)!= 0) {
            std::cout << "Error while locking the mutex reader" << std::endl;
        
        }
    }
    readerC++;
    if (sem_post(&newAccess)!= 0) {
        std::cout << "Error while unlocking the mutex access" << std::endl;

    }
    if (sem_post(&readerAccess)!= 0) {
        std::cout << "Error while unlocking the mutex reader" << std::endl;

    }

}

void secondPartMutexReader(){
    if (sem_wait(&readerAccess)!= 0) {
        std::cout << "Error while locking the mutex reader" << std::endl;

    }
    readerC--;
    if (readerC == 0) {
        if (sem_post(&writeAccess)!= 0) {
            std::cout << "Error while unlocking the mutex reader" << std::endl;

        }
    }
    if (sem_post(&readerAccess)!= 0) {
        std::cout << "Error while unlocking the mutex reader" << std::endl;

    }

}

void firstPartMutexWriter(){
    if (sem_wait(&newAccess)!= 0) {
        std::cout << "Error while locking the mutex access" << std::endl;

    }
    if (sem_wait(&writeAccess)!= 0) {
        std::cout << "Error while locking the mutex reader" << std::endl;

    }

    if (sem_post(&newAccess)!= 0) {
        std::cout << "Error while unlocking the mutex access" << std::endl;

    }
}

void secondPartMutexWriter(){
    if (sem_post(&writeAccess)!= 0) {
        std::cout << "Error while unlocking the mutex reader" << std::endl;
    }
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
        results.push_back("Error: The field is not among {id, lname, section, birthdate}");
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
        bool res = isSearchedStudent(field, value, student);
        if (res) {
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
    if (results.empty() and (isUpdate or isDelete)) {
        return false;
    }
    return true;
}

std::vector<std::string> select(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string field, value;
    int parse;
    if ((parse = parse_selectors(query, field, value))){
        results.push_back("------Select------\n");
        errorParseSelectors(parse, results);
        results.push_back("------------------\n");
        return results;
    }
    
    firstPartMutexReader();
    bool ret = findStudent(database, field, value, results);
    secondPartMutexReader();
    if (not ret) {
        results.push_back("Error: A problem has occured when doing the"
                    "query select. Unknown error\n");
        return results;
    }
    results.push_back(std::to_string(results.size())+" student(s) selected" + '\n');
    return results;
}

std::vector<std::string> update(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string fieldFilter, valueFilter, fieldToUpdate, updateValue;
    int parse;
    if ((parse =parse_update(query, fieldFilter, valueFilter, fieldToUpdate, updateValue))) {
        results.push_back("------Update------\n");
        errorParseUpdate(parse, results);
        results.push_back("------------------\n");
        return results;
    }
    firstPartMutexWriter();
    bool ret = findStudent(database, fieldFilter, valueFilter, results, fieldToUpdate, updateValue);
    secondPartMutexWriter();
    if (not ret) {
        if (results.empty()) {
            results.push_back("Error: There was no such students");
        }
        else {
            results.push_back("Error: A problem has occured when doing the"
                    "query update\n");
        }
        return results;
    }

    return {std::to_string(results.size())+" student(s) updated" + '\n'};
}

std::vector<std::string> insert(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string fname, lname, id, section, birthdate;
    int parse;
    if ((parse = parse_insert(query, fname, lname, id, section, birthdate))) {
        results.push_back("------Insert-----\n");
        errorParseInsert(parse, results);
        results.push_back("------------------\n");
        return results;
    }
    student_t student;
    updateStudent(student, "fname", fname);
    updateStudent(student, "lname", lname);
    updateStudent(student, "id", id);
    updateStudent(student, "section", section);
    updateStudent(student, "birthdate", birthdate);
    firstPartMutexReader();
    std::string field = "id";
    findStudent(database, field, id, results);
    if (not results.empty()) {
        results.push_back("Error: id of student is already in the database\n");
        return results;
    }
    secondPartMutexReader();
    results.push_back(student_to_str(student));
    firstPartMutexWriter();
    db_add(database, student);
    secondPartMutexWriter();
    return results;

}

std::vector<std::string> deletion(database_t *database, std::string query){
    std::vector<std::string> results;
    std::string field, value;
    int parse;
    if ((parse = parse_selectors(query, field, value))) {
        results.push_back("------Delete------\n");
        errorParseSelectors(parse, results);
        results.push_back("------------------\n");
        return results;
    }

    firstPartMutexWriter();
    bool ret = findStudent(database, field, value, results, "delete");
    secondPartMutexWriter();
    if (not ret) {
        if (results.empty()) {
            results.push_back("Error: There was no such students");
        }
        else {
            results.push_back("Error: A problem has occured when doing the"
                    "query delete\n");
        }
        return results;
    }

    return {std::to_string(results.size())+" student(s) deleted" + '\n'};
}
