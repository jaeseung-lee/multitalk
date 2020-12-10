#include "chat.hpp"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <signal.h>

#define PERMS 0664

using namespace std;
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    char status[MAX_STATUS_LENGTH + 1];
} Userinfo;

struct _chat{
    char send[MAX_NAME_LENGTH+1];
    vector<string> receive;
    char chatting[MAX_CHAT_LENGTH+1];
    char time[MAX_NAME_LENGTH+1];
};
typedef struct _chat __chat;

vector<User> Users;
vector<Chat> Chats;

// 현재까지 받은 채팅 로그의 수
int chatSize=0;

// When Ctrl+C is inserted,
void signalHandler(int signum){
    if(signum==SIGINT){
        upload();
        chatOut();
        exit(0);
    }
}

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

void upload(void) {
    int fd;
    fd = open("/tmp/chattings.dat", O_CREAT | O_APPEND | O_RDWR, PERMS);
    if (fd == -1) {
        cout << "open() error!" << endl;
        exit(-1);
    }

    ssize_t wsize = 0;
    vector<User>::iterator it;
    //유저 정보가 담겨있는 벡터를 텍스트 파일에 갱신
    //만약 이미 유저의 이름이 텍스트파일에 있다면 상태메세지만 갱신해주고
    //유저의 이름이 없다면 생성해준다.
    for (it = Users.begin(); it != Users.end(); it++) {
        ssize_t rsize = 0;
        //유저 정보를 담을 구조체를 초기화시켜준다.
        Userinfo *userinfo = new Userinfo;
        memset(userinfo->name, 0x00, MAX_NAME_LENGTH);
        memset(userinfo->status, 0x00, MAX_STATUS_LENGTH);

        //유저 정보를 구조체에 담고, 하나씩 확인
        while (true) {
            rsize = read(fd, (Userinfo *)userinfo, sizeof(Userinfo));
            if (rsize == -1) {
                cout << "upload read() error!" << endl;
                exit(-1);
            }
            //더 이상 읽을 내용이 없다면 정보를 입력한다.
            else if (rsize == 0) {
                wsize = write(fd, &(*it), sizeof(User));
                if (wsize == -1) {
                    cout << "upload write() error!" << endl;
                    exit(1);
                }
                break;
            }
            //읽을 내용이 있는데, 그 내용의 이름과 vector의 이름과 동일하다면
            //상태메세지 갱신
            else {
                if (userinfo->name == (*it).getName()) {
                    (*it).setStatus(userinfo->status);
                    break;
                }
            }
            delete userinfo;
        }
    }
    close(fd);
}

void download(void) {
    int fd;
    fd = open("/tmp/chattings.dat", O_CREAT | O_APPEND | O_RDWR, PERMS);
    if (fd == -1) {
        cout << "open() error!" << endl;
        exit(-1);
    }

    ssize_t rsize = 0;
    while (1) {
        //유저의 정보를 담을 구조체 초기화
        Userinfo *userinfo = new Userinfo;
        memset(userinfo->name, 0x00, MAX_NAME_LENGTH);
        memset(userinfo->status, 0x00, MAX_STATUS_LENGTH);
        if (rsize = read(fd, (Userinfo *)userinfo, sizeof(Userinfo)) == -1) {
            cout << "download error! " << endl;
            exit(-1);
        } else if (rsize == 0) {
            break;
        } else {
            vector<User>::iterator it;
            for (it = Users.begin(); it != Users.end(); it++) {
                //같은게 발견되면 상태메세지만 바꿔준다.
                if (userinfo->name == (*it).getName()) {
                    (*it).setStatus(userinfo->status);
                    break;
                }
            }
            //같은게 발견되지 않으면 벡터에 새로운 유저를 만들어서 넣어준다.
            if (it == Users.end())
                Users.push_back(User(userinfo->name, userinfo->status));
        }
        delete userinfo;
    }
    close(fd);
}

Chat::Chat(){
    memset(this->send,0x00,MAX_NAME_LENGTH);
    memset(this->chatting,0x00,MAX_CHAT_LENGTH);
    memset(this->time,0x00,MAX_NAME_LENGTH);
    receive.clear();
    }

Chat::Chat(string newSend,vector<string> newReceive,string newChatting,string newTime){
    memcpy(this->send,newSend.c_str(),MAX_NAME_LENGTH);
    receive.clear();
    receive.assign(newReceive.begin(),newReceive.end());
    memcpy(this->chatting,newChatting.c_str(),MAX_CHAT_LENGTH);
    memcpy(this->time,newTime.c_str(),MAX_NAME_LENGTH);
}

string Chat::getSend(){return string(this->send);}

vector<string> Chat::getReceive(){return receive;}
string Chat::getChatting(){return string(this->chatting);}
string Chat::getTime(){return string(this->time);}



void chatOut() {
    int fd;
    fd = open("/tmp/chats.txt", O_CREAT | O_APPEND | O_WRONLY, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }

    // chat vector를 클리어해주고, 다시 전부 담는다.
    Chats.clear();

    size_t r_size = 0;
    while (1) {
        __chat temp_chat;
        r_size = read(fd, &temp_chat, sizeof(temp_chat));
        if (r_size == -1)
            break;

        Chats.push_back(Chat((string)temp_chat.send,temp_chat.receive,(string)temp_chat.chatting,(string)temp_chat.time));
    }

    // 현재 어디까지 chatting을 불러왔는지 체크해주는 변수
    chatSize=Chats.size();
}

void chatIn() {
    int fd;
    fd = open("/tmp/chats.txt", O_CREAT | O_APPEND | O_WRONLY, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }
    size_t w_size = 0;

    for (int i = chatSize; i < Chats.size(); i++) {
        __chat temp_chat;
        memcpy(temp_chat.send,Chats[i].getSend().c_str(),MAX_NAME_LENGTH);
        memcpy(temp_chat.chatting,Chats[i].getChatting().c_str(),MAX_CHAT_LENGTH);
        memcpy(temp_chat.time,Chats[i].getTime().c_str(),MAX_NAME_LENGTH);
        temp_chat.receive.clear();
        temp_chat.receive.assign(Chats[i].getReceive().begin(),Chats[i].getReceive().end());

        if ((w_size = write(fd, &temp_chat, sizeof(temp_chat))) == -1) {
            perror("write() error!");
            exit(-1);
        }
    }
}

void out(){
    cout << "Good-bye!" << endl;
    upload();
    chatIn();
}
