#ifndef __CHAT2_HPP__
#define __CHAT2_HPP__

#include <iostream>
#include <stdlib.h>
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
    User(string newName, string newPW);
    User(string newName, string newPW, string newStatus);

    string getName(void);
    string getPW(void);
    string getStatus(void);

    void setName(string newName);
    void setPW(string newPW);
    void setStatus(string newStatus);

  private:
    char name[MAX_NAME_LENGTH + 1];
    char password[MAX_NAME_LENGTH + 1];
    char status[MAX_STATUS_LENGTH + 1];
};

class Chat{
  public:
    Chat();
    Chat(string newSend,string newReceive,string newChatting,string newTime);

    string getSend();
    string getReceive();
    string getChatting();
    string getTime();

   private:
    char send[MAX_NAME_LENGTH+1];
    char receive[MAX_NAME_LENGTH+1];
    char chatting[MAX_CHAT_LENGTH+1];
    char time[MAX_NAME_LENGTH+1];
};

class ChatRoom{
  public:
    ChatRoom();
    ChatRoom(string newRoomName,string newSend,string newReceive);
    string getSend();
    string getReceive();
    string getName();

  private:
    char roomName[MAX_NAME_LENGTH+1];
    char send[MAX_NAME_LENGTH+1];
    char receive[MAX_NAME_LENGTH+1];
};


//유저의 정보를 갱신하는 함수
// 텍스트파일 -> 클라이언트
void download();
// 클라이언트 -> 텍스트파일
void upload();

// 시작메뉴화면
void startMenu();
// 회원가입 화면
void signUp();
// 로그인 화면
void signIn();

/*
>> Main menu
0. Exit
1.
*/
// 메인 메뉴 화면
void mainMenu();

// 다른 사람들 상태메세지 출력
void seeStatus();
// 내 상태메세지 바꾸기
void changeStatus();

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

bool chatMake();

// Check whether the room contains Users[me].getName()
bool is_Member(ChatRoom room);

// Send new chats
/*
>> Chatting
2020-10-17 21:37:00 <name1> >> h
*/
void chatting(ChatRoom room);

// Exit with display "Good-bye"
void out();

// 채팅 로그 받기
void chatIn();

// 채팅 로그 넣기
void chatOut();

#endif //__CHAT2_H__
