#include "chat.h"
#include <iostream>
#include <string>

using namespace std;

User::User() {
    memset(this->name, 0x00, MAX_NAME_LENGTH + 1);
    memset(this->status, 0x00, MAX_STATUS_LENGTH + 1);
}
User::User(string newName) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
    memset(this->status, 0x00, MAX_STATUS_LENGTH + 1);
}
User::User(string newName, string newStatus) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->status, newStatus.c_str(), MAX_STATUS_LENGTH + 1);
}

string User::getName(void) { return string(this->name); }
string User::getStatus(void) { return string(this->status); }
void User::setName(string newName) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH);
}
void User::setStatus(string newStatus) {
    memcpy(this->status, newStatus.c_str(), MAX_STATUS_LENGTH);
}
