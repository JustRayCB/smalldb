#ifndef _STUDENT_HPP
#define _STUDENT_HPP

#include <cstring>
#include <ctime>  // std::tm
#include <functional>
#include <string>

/**
 * Student structure type.
 **/
struct student_t {
  unsigned id;          /** Unique ID **/
  char     fname[64];   /** Firstname **/
  char     lname[64];   /** Lastname **/
  char     section[64]; /** Section **/
  std::tm  birthdate;   /** Birth date **/
};

/**
 * Convert a student to a human-readlable string.
 **/
std::string student_to_str(const student_t &s);

/**
 * Return whether two students are equal or not.
 **/
bool student_equals(const student_t* s1, const student_t* s2);

#endif  // _STUDENT_HPP
