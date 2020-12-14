#include "chat.hpp"
#include "login.hpp" //추가
#include <list> //추가
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
#include <fstream>

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

void chatList() {
    while (1) {
        //메뉴 출력
        int number;
        cout << ">> Chat Lists : " << endl;
        cout << "-1. exit" << endl;
        cout << "0. back" << endl;
        cout << "1. Make new Chat room" << endl;

        //chatList.txt 파일을 읽어서 톡방 갯수에 맞게 출력
        ifstream chatName;
        string chatUserName;
        char selectedChat[100];
        char line[100];
        int num = 2;
        chatName.open("chatList.txt");
        if (chatName.is_open()) {
            while (chatName.getline(line, sizeof(line))) {
                //톡방 이름 chatList.txt에서 줄마다 받아와서 출력
                cout << num << ". "
                     << "Chat Name: " << line << endl;

                //톡방 멤버 받아오기, 각 채팅방에 적혀져 있는 유저 목록 불러오기
                cout << "   Chat Member: ";
                ifstream in(line);
                while (getline(in, chatUserName)) {
                    if (strcmp(chatUserName.c_str(), "END") == 0) {
                        break;
                    }
                    cout << chatUserName << " ";
                }
                cout << endl;
                num++;
            }
        }
        chatName.close();

        //입력 받는곳
        cout << " " << endl;
        cout << ">> select number" << endl;
        cout << "<< ";
        cin >> number;

        //입력 받은 번호에 따라 선택지 갈림
        if (number == 0) {
            cout << "return to back" << endl;
            system("clear");
        } else if (number == 1) {
            chatMake();
            system("clear");
        } else if (number == -1) {
            break;
            exit(-1);
        //0과1과-1을 입력하지 않았다면 톡방으로 들어 가는걸로 간주
        } else {
            //들어갈 톡방 이름을 입력 받음
            cout << ">> Write the name of chat room which you want to "
                    "join(include .txt)"
                 << endl;
            cout << "<< ";
            cin >> selectedChat;
            //입력받은 톡방 열기
            ofstream ofs2(selectedChat, ios::app);
            //이곳 부터 채팅 구현 파트
            ofs2 << "CHATTING" << endl;
            ofs2.close();
            exit(-1);
        }
    }
}

void chatMake() {
    //톡방 이름 받기
    char ch[100];
    char us[100];
    cout << ">> Please put Chat room name + .txt (ex: memo.txt)" << endl;
    cout << "<< ";
    cin >> ch;

    ofstream ofs(ch);
    //벡터 컨테이너에 담긴 겍체에 접근하여 유저 리스트 먼저 보여줌
    cout << endl << ">> ( User List )" << endl;
    for (int i = 0; i < Users.size(); i++)
        cout << i + 1 << ". " << Users[i].getName() << endl;
    //초대할 유저 고르는 파트, 0을 누르면 초대 종료
    cout << ">> Please write users you want to invite: " << endl;
    cout << "   (Write 0 to finish)" << endl;
    while (1) {
        cin >> us;
        if (strcmp(us, "0") == 0) {
            break;
        } else {
            ofs << us << endl;
        }
    }
    //각 톡방 유저 목록을 위해 선 입력
    ofs << "END" << endl;
    ofs << "<Chat room member>" << endl;
    ofs << "-----------------" << endl;
    ofs.close();

    //톡방 이름 txt 따로 저장
    ofstream ofs2("chatList.txt", ios::app);
    ofs2.write(ch, strlen(ch));
    ofs2.put('\n');
    ofs2.close();
}

void signUp() {
    int num = 0;
    string id1 = "";
    string pw1 = "";
    Person* pr;
    list<Person> user;
    list<Person> newuser;

    int fd = open("./logList.dat", O_CREAT | O_APPEND | O_RDWR, PERMS);
    if (fd == -1) {
        cout << "open() error!" << endl;
        exit(-1);
    }

    ssize_t rSize = 0;
    while (rSize = read(fd, (Person*)pr, sizeof(Person))) {
        if (rSize == -1) {
            cout << "error!" << endl;
            exit(-1);
        }
        id1 = pr->getId();
        pw1 = pr->getPw();
        Person info(id1, pw1);
        user.push_back(info);
        num++;
    }

    string id = "";
    string pw = "";
    if (num = 0) {
        cout << "ID (input) : ";
        cin >> id;
        cout << "PW (input) : ";
        cin >> pw;
        Person nuser1(id, pw);
        newuser.push_back(nuser1);

        list<Person>::iterator iter;
        for (iter = newuser.begin(); iter != newuser.end(); ++iter) {
            if (write(fd, &(*iter), sizeof(Person)) == -1) {
                cout << "write error!" << endl;
                exit(-1);
            }
        }
        close(fd);
        cout << "***회원가입 성공!***";

        return;
    }
    else if (num > 0) {
        int count = 1;
        while (count % 2 == 1) {
            int tmp = 1;
            string id2 = "";
            string pw2 = "";

            cout << "ID (input) : ";
            cin >> id2;
            cout << "PW (input) : ";
            cin >> pw2;

            list<Person>::iterator itr;
            for (itr = user.begin(); itr != user.end(); ++itr) {
                if (id2 == itr->getId()) {
                    cout << "***ID 중복! 다시 입력해주세요!***" << endl;
                    tmp = 2;
                }
            }

            if (tmp == 1) {
                Person nuser2(id2, pw2);
                newuser.push_back(nuser2);
                list<Person>::iterator itr2;
                for (itr2 = newuser.begin(); itr2 != newuser.end(); ++itr2) {
                    if (write(fd, &(*itr2), sizeof(Person)) == -1) {
                        cout << "write error!" << endl;
                        exit(-1);
                    }
                }
                close(fd);
                cout << "***회원가입 성공!***";
                count = 2;
            }
        }

        return;
    }

}

void signIn() {
    cout << "***로그인***" << endl;
    int tmp = 0;
    string id1 = "";
    string pw1 = "";
    Person* pr;
    list<Person> user;
    list<Person> loguser;

    int fd = open("./logList.dat", O_CREAT | O_APPEND | O_RDWR, PERMS);
    if (fd == -1) {
        cout << "open() error!" << endl;
        exit(-1);
    }

    ssize_t rSize = 0;
    while (rSize = read(fd, (Person*)pr, sizeof(Person))) {
        if (rSize == -1) {
            cout << "error!" << endl;
            exit(-1);
        }

        id1 = pr->getId();
        pw1 = pr->getPw();
        Person info(id1, pw1);
        user.push_back(info);
    }

    while (tmp == 0) {
        string id2 = "";
        string pw2 = "";
        cout << "ID : ";
        cin >> id2;
        cout << "PW : ";
        cin >> pw2;

        list<Person>::iterator itr;
        for (itr = user.begin(); itr != user.end(); ++itr) {
            if (id2 == itr->getId()) {
                if (pw2 == itr->getPw()) {
                    tmp = 1;
                }
            }
        }

        if (tmp == 0) {
            cout << "***ID 혹은 PW 가 정확하지 않습니다!***" << endl;
        }
    }

    if (tmp == 1) {
        cout << "***로그인 성공!***" << endl;

        return;
        //여기서 시작메뉴창으로 넘어가야함
    }

}