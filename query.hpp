#ifndef _QUERY_HPP
#define _QUERY_HPP

#include <tuple>
#include <vector>
#include <string>

#include "db.hpp"


/*
 * @brief Function that will manage the select command
 *
 * @param database: pointer to the database struct
 * @param query: Command of the user
 *
 * @return vector of string with all the string we want to display on the client
 */
std::vector<std::string> select(database_t *database, std::string query);

/*
 * @brief: Function that will insert a student in a database if he is not
 * already here
 *
 * @param database: pointer to the database
 * @param query: query of the user
 *
 * @return vector of string with all the string we want to display on the client
 */
std::vector<std::string> insert(database_t *database, std::string query);
/*
 * @brief: Function that will manage the delete command
 *
 * @param database: pointer to the database
 * @param query: query of the user
 *
 * @return vector of string with all the string we want to display on the client
 * @return: query_result_t struct with the result
 */
std::vector<std::string> deletion(database_t *database, std::string query);

/*
 * @brief: Function that will manage the update command from the database
 *
 * @param database: pointer to the database
 * @param query: query of the user
 *
 * @return vector of string with all the string we want to display on the client
 */
std::vector<std::string> update(database_t *database, std::string query);

/*
 * @brief Function that will find all the student that match with field=value
 * and update or delete it if it's necessary
 *
 * @param database: pointer to the database
 * @param field: Field we want to search in the database
 * @param value: Value of the field we want to search in the database
 * --------Only with the update function---------------
 * @param fieldToUpdate: Field we want to update
 * @param updateValue: Value we want to the field we need to update
 * --------Only with the delete function---------------
 * @param fieldToUpdate: flag to say that we want to delete
 *
 * @return true if the functin was successfull, false otherwise
 */
bool findStudent(database_t *database, std::string &field, std::string &value, std::vector<std::string> &results,
        const std::string &fieldToUpdate = "", const std::string &updateValue = "");

/*
 * @brief: Function that will update a student
 *
 * @param student: Student we want to update the field with the value
 * @param fieldToUpdate: field we want to update
 * @param updateValue: value we want to update the field with
 */
void updateStudent(student_t &student, const std::string &fieldToUpdate, const std::string &updateValue);

/*
 * @brief: Function that will search if it's the student we are
 * searching for
 * @param field: Field we want to search for
 * @param value: value we want to search for
 * @param student: Student we want to search
 *
 * @return true if it's the student false otherwise
 */
bool isSearchedStudent(const std::string &field, const std::string &value, const student_t &student);

/*
 * @brief: Function that correspond to the first part of
 * locking and unlocking semaphores for the read operation in the pseudocode.
 */
void firstPartMutexReader();
/*
 * @brief: Function that correspond to the second part of
 * locking and unlocking semaphores for the read operation in the pseudocode.
 */
void secondPartMutexReader();
/*
 * @brief: Function that correspond to the first part of
 * locking and unlocking semaphores for the write operation in the pseudocode.
 */
void firstPartMutexWriter();
/*
 * @brief: Function that correspond to the second part of
 * locking and unlocking semaphores for the write operation in the pseudocode.
 */
void secondPartMutexWriter();
#endif
