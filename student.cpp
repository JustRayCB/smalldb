#include "student.hpp"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <ctime>




std::string student_to_str(const student_t &s){
    char charBuffer[256];
    std::string myBuffer;
    snprintf(charBuffer, 256, "%.9u: %s %s in section %s, born on the %.2d/%.2d/%.2d\n",
            s.id, s.fname, s.lname, s.section, s.birthdate.tm_mday, s.birthdate.tm_mon + 1, s.birthdate.tm_year + 1900);

    myBuffer = charBuffer;
    return myBuffer;
}

bool student_equals(const student_t *s1, const student_t *s2) {
  return s1->id == s2->id && strcmp(s1->fname, s2->fname) == 0 &&
         strcmp(s1->lname, s2->lname) == 0 &&
         strcmp(s1->section, s2->section) == 0 &&
         s1->birthdate.tm_mday == s2->birthdate.tm_mday &&
         s1->birthdate.tm_mon == s2->birthdate.tm_mon &&
         s1->birthdate.tm_year == s2->birthdate.tm_year;
}

