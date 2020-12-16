#include "login.hpp"
#include <string.h>
#include <string>

using namespace std;

Person::Person() {
    memset(this->id, 0x00, MAX_LEN + 1);
    memset(this->pw, 0x00, MAX_LEN + 1);
}

Person::Person(string id, string pw) {
    memcpy(this->id, id.c_str(), MAX_LEN);
    memcpy(this->pw, pw.c_str(), MAX_LEN);
}

void Person::setId(string id) { memcpy(this->id, id.c_str(), MAX_LEN); }

void Person::setPw(string pw) { memcpy(this->pw, pw.c_str(), MAX_LEN); }

string Person::getId(void) { return string(this->id); }

string Person::getPw(void) { return string(this->pw); }
