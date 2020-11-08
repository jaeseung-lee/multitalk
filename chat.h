#ifndef __CHAT_H__
#define __CHAT_H__

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>
#define MAX_NAME_LENGTH 32
#define MAX_STATUS_LENGTH 256
#define MAX_CHAT_LENGTH 256
using namespace std;
/*
  User 이름;
    {
      이름
      상태메세지
    }
*/
class User {
  public:
    User();
    //처음에 아이디 만들었을 때 호출할 것
    User(string newName);
    //다른 사용자가 가입했을 때 불러올 것
    User(string newName, string newStatus);
    string getName(void);
    string getStatus(void);
    void setStatus(string newName);
    void setName(string newStatus);

  private:
    char name[MAX_NAME_LENGTH + 1];
    char status[MAX_STATUS_LENGTH + 1];
};

struct __Chat {
    char send[MAX_NAME_LENGTH + 1];
    vector<char[MAX_NAME_LENGTH + 1]> receive;
    char chatting[MAX_CHAT_LENGTH + 1];
    int time;
};

typedef struct __Chat Chat;
vector<User> Users;
vector<Chat> Chats;

void download();
void upload();
void startMenu();
void signUp();
void signIn();
void mainMenu();
void seeStatus();
void changeStatus();
void chatList();
void chatting();
void out();

#endif __CHAT_H__
