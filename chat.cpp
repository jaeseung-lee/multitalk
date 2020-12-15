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
#include <fstream>

#define PERMS 0664

using namespace std;
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    char password[MAX_NAME_LENGTH + 1];
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
        exit(0);
    }
}

User::User() {
    memset(this->name, 0x00, MAX_NAME_LENGTH + 1);
    memset(this->password, 0x00, MAX_NAME_LENGTH + 1);
}

//현재 접속해 있는 유저의 정보를 담고있는 클래스의 번호
//Users[me];
int me;

User::User(string newName, string newPW) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->password, newPW.c_str(), MAX_NAME_LENGTH + 1);
}

User::User(string newName, string newPW, string newStatus){
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->password, newPW.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->status, newStatus.c_str(), MAX_STATUS_LENGTH+1);
}

string User::getName(void) { return string(this->name); }
string User::getPW(void) { return string(this->password);}
string User::getStatus(void) { return string(this->status); }

void User::setName(string newName){
    memcpy(this->name, newName.c_str(),MAX_NAME_LENGTH);
}

void User::setPW(string newPW){
    memcpy(this->password, newPW.c_str(),MAX_NAME_LENGTH);
}

void User::setStatus(string newStatus) {
    memcpy(this->status, newStatus.c_str(), MAX_STATUS_LENGTH);
}

void upload(void) {
    int fd;
    remove("tmp/chattings.dat");

    fd = open("/tmp/chattings.dat", O_CREAT | O_RDWR, PERMS);
    if (fd == -1) {
        cout << "open() error!" << endl;
        exit(-1);
    }

    ssize_t wsize = 0;
    //유저 정보가 담겨있는 벡터를 텍스트 파일에 갱신
    //만약 이미 유저의 이름이 텍스트파일에 있다면 상태메세지만 갱신해주고
    //유저의 이름이 없다면 생성해준다.
    for (int i=0; i<Users.size(); i++) {
        ssize_t rsize = 0;
        //유저 정보를 담을 구조체를 초기화시켜준다.
        Userinfo *userinfo = new Userinfo;
        memcpy(userinfo->name, Users[i].getName().c_str(), MAX_NAME_LENGTH);
        memcpy(userinfo->password, Users[i].getPW().c_str(), MAX_NAME_LENGTH);
        memcpy(userinfo->status, Users[i].getStatus().c_str(), MAX_STATUS_LENGTH);

        wsize = write(fd, userinfo, sizeof(User));
        if(wsize==-1){
            perror("write() error\n");
            exit(-1);
        }
        delete userinfo;
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
        memset(userinfo->password, 0x00, MAX_NAME_LENGTH);
        memset(userinfo->status, 0x00, MAX_STATUS_LENGTH);

        if ((rsize = read(fd, (Userinfo *)userinfo, sizeof(Userinfo)) )== -1) {
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
                Users.push_back(User(userinfo->name, userinfo->password, userinfo->status));
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
    download();
    string id = "";
    string pw = "";

    while(1) {
        cout << "ID (input) : ";
        cin >> id;

        // 회원가입시 동일한 아이디가 있는지 확인
        int i;
        for(i=0;i<Users.size();i++){
            if(Users[i].getName()==id){
                cout << "동일한 아이디가 존재합니다." << endl;
                break;
            }
        }

        if(i==Users.size())
            break;
    }

    cout << "PW (input) : ";
    cin >> pw;

    Users.push_back(User(id,pw));
    cout << "***회원가입 성공!***"<<endl;
    upload();
    startMenu();
    return;
}

void signIn() {
    download();

    cout << "***로그인***" << endl;
    int tmp = 0;
    string id1 = "";
    string pw1 = "";

    vector<User>::iterator itr;
    int count=0;

    while (tmp == 0) {
        string id2 = "";
        string pw2 = "";
        cout << "ID : ";
        cin >> id2;
        cout << "PW : ";
        cin >> pw2;

        for (itr = Users.begin(); itr != Users.end(); ++itr){
            cout << "name: " << (*itr).getName() << " PW: " << (*itr).getPW() << endl;
        }

        for (itr = Users.begin(); itr != Users.end(); ++itr) {
            if (id2 == (*itr).getName() && pw2==(*itr).getPW()) {
                tmp = 1;
            }
        }

        if (tmp == 0) {
            cout << "***ID 혹은 PW 가 정확하지 않습니다!***" << endl;
        }
    }

    if (tmp == 1) {
        cout << "***로그인 성공!***" << endl;
        me=count;
        mainMenu();
        return;
    }
}

void startMenu(){
    while(1){
        cout << "채팅 프로그램에 오신 것을 환영합니다." << endl;
        cout << "1. 로그인" << endl;
        cout << "2. 회원가입" << endl;

        int choice=0;
        cin >> choice;

        if(choice == 1){
            signIn();
            return;
        }

        if(choice == 2) {
            signUp();
            return;
        }
    }
}

void mainMenu(){
    while(1){
        cout << "****메인 메뉴****" << endl;
        cout << "0. 종료" << endl;
        cout << "1. 상태메세지 보기" << endl;
        cout << "2. 나의 상태메세지 수정하기" << endl;
        cout << "3. 채팅 목록 열기" << endl;

        int choice=0;
        cin >> choice;

        if(choice == 0){
            chatOut();
        }

        if(choice == 1){
            seeStatus();
        }

        if(choice == 2) {
            changeStatus();
        }

        if(choice == 3){
            chatList();
            return;
        }
    }
}

void seeStatus() {
    download();
    cout << "***상태메세지 목록***" << endl;
	vector<User>::iterator it;
	for (it = Users.begin(); it != Users.end(); it++) {
		cout << (*it).getName();
		cout << "\t <[ " << (*it).getStatus() << " ]" << endl;
		cout << "---------------------------------" << endl;
	}
    return;
}

void changeStatus(){
    download();
    cout << "***상태메세지 수정하기***" << endl;
    cout << "현재 상태메세지 : " << "\t <[ " << Users[me].getStatus() << " ]" << endl;
    cout << "바꿀 상태메세지를 입력해주세요." << endl;
    char temp[MAX_STATUS_LENGTH+1];
    scanf("%257s",temp);
    Users[me].setStatus((string)temp);
    cout << "바뀐 상태메세지 : " << "\t <[ " << Users[me].getStatus() << " ]" << endl;
    upload();
    return;
}
