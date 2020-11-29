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

//유저의 정보를 갱신하는 함수
// 텍스트파일 -> 클라이언트
void download();
// 클라이언트 -> 텍스트파일
void upload();

// 시작메뉴화면
void startMenu();
// 로그인 화면
void signUp();
// 회원가입 화면
void signIn();

/*
>> Main menu

*/
// 메인 메뉴 화면
void mainMenu();

// 다른 사람들 상태메세지 출력
void seeStatus();
// 내 상태메세지 바꾸기
void changeStatus();


// Send new chats
void sendChat();

//Show all chat rooms.
/*
>> Chat Lists :
0. back
1. with <name1>
2. with <name2>
3. with <name3>, <name4>, <name5>

>> select number
<<
*/
void chatList();

// Show all messages which is sent or receieved in time order.
void chatting();

// Exit with display "Good-bye"
void out();

#endif //__CHAT_H__
