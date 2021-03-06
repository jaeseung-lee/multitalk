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
    char receive[MAX_MEMBER][MAX_NAME_LENGTH+1];
    char chatting[MAX_CHAT_LENGTH+1];
    char time[MAX_NAME_LENGTH+1];
};
typedef struct _chat __chat;

struct _room{
    char roomName[MAX_NAME_LENGTH+1];
    char members[MAX_MEMBER][MAX_NAME_LENGTH+1];
};
typedef struct _room __room;

vector<User> Users;
vector<Chat> Chats;
vector<ChatRoom> Rooms;

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

    for (int i=0; i<Users.size(); i++) {
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
    memset(this->receive,0x00,MAX_BUFFER);
}

Chat::Chat(string newSend,vector<string> newReceive,string newChatting,string newTime){
    memcpy(this->send,newSend.c_str(),MAX_NAME_LENGTH);
    memcpy(this->chatting,newChatting.c_str(),MAX_CHAT_LENGTH);
    memcpy(this->time,newTime.c_str(),MAX_NAME_LENGTH);
    for(int i=0;i<newReceive.size();i++){
        memcpy(this->receive[i],newReceive[i].c_str(),MAX_NAME_LENGTH);
    }
}

Chat::Chat(string newSend,char newReceive[MAX_MEMBER][MAX_NAME_LENGTH+1],string newChatting, string newTime){
    memcpy(this->send,newSend.c_str(),MAX_NAME_LENGTH);
    memcpy(this->chatting,newChatting.c_str(),MAX_CHAT_LENGTH);
    memcpy(this->time,newTime.c_str(),MAX_NAME_LENGTH);
    memcpy(this->receive,newReceive,MAX_BUFFER);
}

string Chat::getSend(){return string(this->send);}

vector<string> Chat::getReceive(){
    vector<string> result;
    for(int i=0;i<MAX_MEMBER;i++){
        result.push_back(string(receive[i]));
    }
    return result;
}
string Chat::getChatting(){return string(this->chatting);}
string Chat::getTime(){return string(this->time);}

ChatRoom::ChatRoom(){
    memcpy(this->roomName,"이름 없음",MAX_NAME_LENGTH);
}

ChatRoom::ChatRoom(string newRoomName,vector<string> newMembers){
    memcpy(this->roomName,newRoomName.c_str(),MAX_NAME_LENGTH);
    for(int i=0;i<newMembers.size();i++){
        memcpy(this->members[i],newMembers[i].c_str(),MAX_NAME_LENGTH);
    }
}
ChatRoom::ChatRoom(string newRoomName,char newMembers[MAX_MEMBER][MAX_NAME_LENGTH+1]){
    memcpy(this->roomName,newRoomName.c_str(),MAX_NAME_LENGTH);
    memcpy(this->members,newMembers,MAX_BUFFER);
}

vector<string> ChatRoom::getMembers(){
    vector<string> result;
    for(int i=0;i<MAX_MEMBER;i++){
        result.push_back(string(members[i]));
    }
    return result;
}

string ChatRoom::getName(){return (string)this->roomName;}

void chatOut() {
    int fd;
    fd = open("/tmp/chats.txt", O_CREAT | O_RDWR, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }

    // chat vector를 클리어해주고, 다시 전부 담는다.
    Chats.clear();

    size_t r_size = 0;
    while (1) {
        __chat *temp_chat = new __chat;
        memset(temp_chat->send,0x00,MAX_NAME_LENGTH);
        memset(temp_chat->chatting,0x00,MAX_CHAT_LENGTH);
        memset(temp_chat->time,0x00,MAX_NAME_LENGTH);
        memset(temp_chat->receive,0x00,MAX_BUFFER);

        r_size = read(fd, (__chat*)temp_chat, sizeof(temp_chat));
        if(r_size==-1){
            perror("read() error!");
            exit(-1);
        }
        else if (r_size == 0)
            break;
        else
            Chats.push_back(Chat((string)temp_chat->send,temp_chat->receive,(string)temp_chat->chatting,(string)temp_chat->time));

        delete temp_chat;
    }
    close(fd);

    int fd2;
    fd2=open("/tmp/chatList.txt",O_CREAT|O_RDONLY,PERMS);
    if(fd2==-1) {
        perror("open error!");
        exit(-1);
    }

    Rooms.clear();
    r_size=0;

    while(1){
        __room *temp_room = new __room;
        memset(temp_room->roomName,0x00,MAX_NAME_LENGTH);
        memset(temp_room->members,0x00,MAX_BUFFER);
        r_size=read(fd2,(__room*)temp_room,sizeof(temp_room));
        if(r_size==-1){
            perror("read() error!");
            exit(-1);
        }
        else if(r_size==0)
            break;
        else
            Rooms.push_back(ChatRoom(temp_room->roomName,temp_room->members));
    }

    close(fd2);
}

void chatIn() {
    int fd;
    remove("/tmp/chats.txt");

    fd = open("/tmp/chats.txt", O_CREAT | O_WRONLY, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }
    size_t w_size = 0;
    for (int i = 0; i < Chats.size(); i++) {
        __chat *temp_chat = new __chat;
        memcpy(temp_chat->send,Chats[i].getSend().c_str(),MAX_NAME_LENGTH);
        memcpy(temp_chat->chatting,Chats[i].getChatting().c_str(),MAX_CHAT_LENGTH);
        memcpy(temp_chat->time,Chats[i].getTime().c_str(),MAX_NAME_LENGTH);
        vector<string> result=Chats[i].getReceive();
        for(int j=0;j<MAX_MEMBER;j++){
            memcpy(temp_chat->receive[j],result[j].c_str(),MAX_NAME_LENGTH);
        }

        if ((w_size = write(fd, &temp_chat, sizeof(temp_chat))) == -1) {
            perror("write() error!");
            exit(-1);
        }
        delete temp_chat;
    }
    close(fd);

    remove("/tmp/chatList.txt");
    int fd2;
    fd2=open("/tmp/chatList.txt", O_CREAT | O_WRONLY, PERMS);
    if (fd2==-1){
        perror("open error!");
        exit(-1);
    }

    w_size=0;

    for (int i = 0; i < Rooms.size(); i++) {
        __room *temp_room = new __room;
        memcpy(temp_room->roomName,Rooms[i].getName().c_str(),MAX_NAME_LENGTH);
        vector<string> result=Chats[i].getReceive();
        for(int j=0;j<MAX_MEMBER;j++){
            memcpy(temp_room->members[j],result[j].c_str(),MAX_NAME_LENGTH);
        }

        if ((w_size = write(fd, &temp_room, sizeof(temp_room))) == -1) {
            perror("write() error!");
            exit(-1);
        }
        delete temp_room;
    }
    close(fd2);
}

void out(){
    cout << "Good-bye!" << endl;
    upload();
    exit(-1);
}

void chatList() {
    while (1) {
        //chatIn();
        //메뉴 출력
        int number;
        cout << ">> Chat Lists : " << endl;
        cout << "-1. exit" << endl;
        cout << "0. back" << endl;
        cout << "1. Make new Chat room" << endl;

        int count=2;
        //members에 로그인 되어있는사람(Users[me])이 포함되어있는 채팅방만 출력
        for(int i=0;i<Rooms.size();i++){
            if(is_Member(Rooms[i])){
                cout << count << ". " << Rooms[i].getName() << endl;
                count++;
            }
        }

        cin >> number;
        if(number==-1)
            out();

        if(number==0){
            mainMenu();
            return;
        }

        if(number==1){
            while(!chatMake()){}
            cout << "chatting" << endl;
        }
    }
}

bool is_Member(ChatRoom room){
    vector<string> temp_members=room.getMembers();
    for(int j=0;j<temp_members.size();j++){
        if(temp_members[j]==Users[me].getName())
            return true;
    }
    return false;
}

void chatting() {

}

bool chatMake() {
   // chatIn();
    vector<string> tempMembers;
    tempMembers.push_back(Users[me].getName());
    string ch;
    cout << ">> Please put Chat room name" << endl;
    cout << "<< ";
    cin >> ch;

    cout << endl << ">> ( User List )" << endl;
    int count =1;
    for (int i = 0; i < Users.size(); i++){
        if(me!=i){
            cout << count << ". " << Users[i].getName() << endl;
            count++;
        }
    }

    //초대할 유저 고르는 파트, 0을 누르면 초대 종료
    cout << ">> Please write users you want to invite: " << endl;
    cout << "   (Write 0 to finish)" << endl;

    string us;
    while(1){
        cin >> us;
        if(us=="0")
            break;
        //us에 저장되어있는 이름이 실제로 있는 이름인지 확인하고 저장
        int i;
        for(i=0;i<tempMembers.size();i++){
            if(tempMembers[i]==us){
                cout << "Duplicative invitation - Failed to invite. :(" << endl;
                goto here;
                break;
            }
        }
        for(i=0;i<Users.size();i++){
            if(us==Users[i].getName()){
                tempMembers.push_back(us);
                cout << "Successfully invited!" << endl;
                break;
            }
        }

        if(i==Users.size())
            cout << "Failed to invite. :(" << endl;
        here:
        us.clear();
    }

    // 기존에 있는 톡방인지 확인
    for(int i=0;i<Rooms.size();i++) {
        if(Rooms[i].getMembers()==tempMembers){
            cout << "구성원이 동일한 채팅방이 존재합니다." << endl;
            return false;
        }
    }
    Rooms.push_back(ChatRoom(ch,tempMembers));
    //chatOut();
    return true;
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
                break;
            }
            count++;
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
    cout << endl;
    cout << Users[me].getName() << " 님, 안녕하세요!" << endl;

    while(1){
        cout << endl;
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
