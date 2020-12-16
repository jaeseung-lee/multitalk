#include "chat2.hpp"
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define PERMS 0664

using namespace std;
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    char password[MAX_NAME_LENGTH + 1];
    char status[MAX_STATUS_LENGTH + 1];
} Userinfo;

typedef struct {
    char send[MAX_NAME_LENGTH + 1];
    char receive[MAX_NAME_LENGTH + 1];
    char chatting[MAX_CHAT_LENGTH + 1];
    char time[MAX_NAME_LENGTH + 1];
} __chat;

typedef struct {
    char roomName[MAX_NAME_LENGTH + 1];
    char send[MAX_NAME_LENGTH + 1];
    char receive[MAX_NAME_LENGTH + 1];
} __room;

vector<User> Users;
vector<Chat> Chats;
vector<ChatRoom> Rooms;

// When Ctrl+C is inserted,
void signalHandler(int signum) {
    if (signum == SIGINT) {
        upload();
        exit(0);
    }
}

User::User() {
    memset(this->name, 0x00, MAX_NAME_LENGTH + 1);
    memset(this->password, 0x00, MAX_NAME_LENGTH + 1);
}

//현재 접속해 있는 유저의 정보를 담고있는 클래스의 번호
// Users[me];
int me;

User::User(string newName, string newPW) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->password, newPW.c_str(), MAX_NAME_LENGTH + 1);
}

User::User(string newName, string newPW, string newStatus) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->password, newPW.c_str(), MAX_NAME_LENGTH + 1);
    memcpy(this->status, newStatus.c_str(), MAX_STATUS_LENGTH + 1);
}

string User::getName(void) { return string(this->name); }
string User::getPW(void) { return string(this->password); }
string User::getStatus(void) { return string(this->status); }

void User::setName(string newName) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH);
}

void User::setPW(string newPW) {
    memcpy(this->password, newPW.c_str(), MAX_NAME_LENGTH);
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

    for (int i = 0; i < Users.size(); i++) {
        Userinfo *userinfo = new Userinfo;
        memcpy(userinfo->name, Users[i].getName().c_str(), MAX_NAME_LENGTH);
        memcpy(userinfo->password, Users[i].getPW().c_str(), MAX_NAME_LENGTH);
        memcpy(userinfo->status, Users[i].getStatus().c_str(),
               MAX_STATUS_LENGTH);

        wsize = write(fd, userinfo, sizeof(User));
        if (wsize == -1) {
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

        if ((rsize = read(fd, (Userinfo *)userinfo, sizeof(Userinfo))) == -1) {
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
                Users.push_back(
                    User(userinfo->name, userinfo->password, userinfo->status));
        }
        delete userinfo;
    }
    close(fd);
}

Chat::Chat() {
    memset(this->send, 0x00, MAX_NAME_LENGTH);
    memset(this->chatting, 0x00, MAX_CHAT_LENGTH);
    memset(this->time, 0x00, MAX_NAME_LENGTH);
    memset(this->receive, 0x00, MAX_NAME_LENGTH);
}

Chat::Chat(string newSend, string newReceive, string newChatting,
           string newTime) {
    memcpy(this->send, newSend.c_str(), MAX_NAME_LENGTH);
    memcpy(this->chatting, newChatting.c_str(), MAX_CHAT_LENGTH);
    memcpy(this->time, newTime.c_str(), MAX_NAME_LENGTH);
    memcpy(this->receive, newReceive.c_str(), MAX_NAME_LENGTH);
}

string Chat::getSend() { return string(this->send); }
string Chat::getReceive() { return string(this->receive); }
string Chat::getChatting() { return string(this->chatting); }
string Chat::getTime() { return string(this->time); }

ChatRoom::ChatRoom() { memcpy(this->roomName, "이름 없음", MAX_NAME_LENGTH); }

ChatRoom::ChatRoom(string newRoomName, string newSend, string newReceive) {
    memcpy(this->roomName, newRoomName.c_str(), MAX_NAME_LENGTH);
    memcpy(this->send, newSend.c_str(), MAX_NAME_LENGTH);
    memcpy(this->receive, newReceive.c_str(), MAX_NAME_LENGTH);
}

string ChatRoom::getSend() { return (string)this->send; }
string ChatRoom::getReceive() { return (string)this->receive; }
string ChatRoom::getName() { return (string)this->roomName; }

void chatIn() {
    int fd;
    fd = open("/tmp/chats.dat", O_CREAT | O_RDWR, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }

    // chat vector를 클리어해주고, 다시 전부 담는다.
    Chats.clear();

    size_t r_size = 0;
    while (1) {
        __chat *temp_chat = new __chat;
        memset(temp_chat->send, 0x00, MAX_NAME_LENGTH);
        memset(temp_chat->chatting, 0x00, MAX_CHAT_LENGTH);
        memset(temp_chat->time, 0x00, MAX_NAME_LENGTH);
        memset(temp_chat->receive, 0x00, MAX_NAME_LENGTH);

        r_size = read(fd, (__chat *)temp_chat, sizeof(__chat));
        if (r_size == -1) {
            perror("read() error!");
            exit(-1);
        } else if (r_size == 0)
            break;
        else {
            Chats.push_back(
                Chat((string)temp_chat->send, (string)temp_chat->receive,
                     (string)temp_chat->chatting, (string)temp_chat->time));
        }

        delete temp_chat;
    }
    close(fd);

    int fd2;
    fd2 = open("/tmp/chatList.dat", O_CREAT | O_RDWR, PERMS);
    if (fd2 == -1) {
        perror("open error!");
        exit(-1);
    }
    Rooms.clear();
    r_size = 0;

    while (1) {
        __room *temp_room = new __room;
        memset(temp_room->roomName, 0x00, MAX_NAME_LENGTH);
        memset(temp_room->send, 0x00, MAX_NAME_LENGTH);
        memset(temp_room->receive, 0x00, MAX_NAME_LENGTH);

        r_size = read(fd2, (__room *)temp_room, sizeof(__room));
        if (r_size == -1) {
            perror("read() error!");
            exit(-1);
        } else if (r_size == 0)
            break;
        else
            Rooms.push_back(ChatRoom((string)temp_room->roomName,
                                     (string)temp_room->send,
                                     (string)temp_room->receive));
    }
    close(fd2);
}

void chatOut() {
    int fd;
    remove("/tmp/chats.dat");

    fd = open("/tmp/chats.dat", O_CREAT | O_RDWR, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }
    ssize_t w_size = 0;

    for (int i = 0; i < Chats.size(); i++) {
        __chat *temp_chat = new __chat;
        memcpy(temp_chat->send, Chats[i].getSend().c_str(), MAX_NAME_LENGTH);
        memcpy(temp_chat->chatting, Chats[i].getChatting().c_str(),
               MAX_CHAT_LENGTH);
        memcpy(temp_chat->time, Chats[i].getTime().c_str(), MAX_NAME_LENGTH);
        memcpy(temp_chat->receive, Chats[i].getReceive().c_str(),
               MAX_NAME_LENGTH);

        w_size = write(fd, temp_chat, sizeof(Chat));
        cout << "chats" << Chats[i].getSend() << endl;
        if (w_size == -1) {
            perror("write() error!\n");
            exit(-1);
        }
        delete temp_chat;
    }
    close(fd);

    remove("/tmp/chatList.dat");
    int fd2;
    fd2 = open("/tmp/chatList.dat", O_CREAT | O_WRONLY | O_TRUNC, PERMS);
    if (fd2 == -1) {
        perror("open error!");
        exit(-1);
    }

    w_size = 0;

    for (int i = 0; i < Rooms.size(); i++) {
        __room *temp_room = new __room;
        memcpy(temp_room->roomName, Rooms[i].getName().c_str(),
               MAX_NAME_LENGTH);
        memcpy(temp_room->send, Rooms[i].getSend().c_str(), MAX_NAME_LENGTH);
        memcpy(temp_room->receive, Rooms[i].getReceive().c_str(),
               MAX_NAME_LENGTH);
        if ((w_size = write(fd, temp_room, sizeof(__room))) == -1) {
            perror("write() error!");
            exit(-1);
        }
        delete temp_room;
    }
    system("clear");
    close(fd2);
    cout << "+====================================+" << endl;
    cout << "              Chat List              " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
}

void out() {
    upload();
    chatOut();
    system("clear");
    cout << "+===================================+" << endl << endl;
    cout << "        THANK YOU FOR USING" << endl;
    cout << "             Good-bye!" << endl << endl;
    cout << "+===================================+" << endl << endl;
    exit(-1);
}

void chatList() {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "              Chat List              " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    while (1) {
        chatIn();
        //메뉴 출력
        int number;
        cout << "-1. 종료" << endl;
        cout << "0. 메인메뉴" << endl;
        cout << "1. 채팅방 생성" << endl;

        int count = 2;

        for (int i = 0; i < Rooms.size(); i++) {
            if (Rooms[i].getSend() == Users[me].getName() ||
                Rooms[i].getReceive() == Users[me].getName()) {
                cout << count << ". " << Rooms[i].getName() << endl;
                cout << "   MEMBERS: " << Rooms[i].getReceive() << ", "
                     << Rooms[i].getSend() << endl;
            }
        }
        cout << endl;
        cout << "Input: ";
        cin >> number;
        cin.ignore(100, '\n');

        if (number == -1)
            out();

        else if (number == 0) {
            mainMenu();
            return;
        }

        else if (number == 1) {
            while (!chatMake()) {
            }
        }
        // number가 2이상인 수 일 때
        else {
            //그 번호가 어떤 채팅방을 가리키고 있는지 확인한다.
            count = 1;
            for (int i = 0; i < Rooms.size(); i++) {
                if (Rooms[i].getSend() == Users[me].getName() ||
                    Rooms[i].getReceive() == Users[me].getName()) {
                    count++;
                    // cout << "count : " << count << endl;
                }
                if (number == count)
                    chatting(Rooms[i]);
                break;
            }
        }
    }
}

void chatting(ChatRoom room) {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "              Chat room              " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    chatIn();
    string other;
    // cout << "방"<<room.getSend() << " "<<room.getReceive() << endl;
    if (room.getSend() == Users[me].getName())
        other = room.getReceive();
    else
        other = room.getSend();

    cout << endl << "채팅방에 들어왔습니다." << endl;
    cout << "이 방에는 " << Users[me].getName() << ", " << other
         << "님이 속해있습니다." << endl;
    cout << endl;

    for (int j = 0; j < Chats.size(); j++) {
        // 받은사람이 나, 보낸사람이 상대방
        if ((Chats[j].getReceive() == other) &&
            (Chats[j].getSend() == Users[me].getName())) {
            cout << Chats[j].getTime() << " [ " << Users[me].getName() << " ] "
                 << Chats[j].getChatting() << endl;
        }
        // 받은사람이 상대방, 보낸사람이 나
        if ((Chats[j].getReceive() == Users[me].getName()) &&
            (Chats[j].getSend() == other)) {
            cout << Chats[j].getTime() << " [ " << other << " ] "
                 << Chats[j].getChatting() << endl;
        }
    }

    while (1) {
        cout << endl;
        cout << "채팅 보내기를 종료하시려면 q를 입력해주세요." << endl;
        cout << "입력 : " << endl;
        string temp;
        getline(cin, temp);
        if (temp == "q")
            break;
        else {
            time_t rawtime;
            time(&rawtime);
            Chats.push_back(Chat(Users[me].getName(), other, temp,
                                 (string)ctime(&rawtime)));
            cout << ctime(&rawtime);
            cout << "입력완료!" << endl;
        }
    }
    cout << endl;
    chatOut();
}

bool chatMake() {
    chatIn();
    string ch;
    cout << endl;
    cout << ">> 채팅방의 이름을 입력해주세요." << endl;
    cout << "<< ";
    getline(cin, ch);

    cout << endl << ">> ( 유저 목록 )" << endl;
    int count = 1;
    for (int i = 0; i < Users.size(); i++) {
        if (me != i) {
            cout << count << ". " << Users[i].getName() << endl;
            count++;
        }
    }

    cout << endl;
    cout << ">> 상대방을 입력해주세요 : " << endl;

    string us;

    while (1) {
        getline(cin, us);
        // us에 저장되어있는 이름이 실제로 있는 이름인지 확인하고 저장
        int i;
        for (i = 0; i < Users.size(); i++) {
            if (us == Users[i].getName()) {
                cout << endl;
                cout << "+----------WARNING---------+" << endl;
                cout << endl;
                cout << "          초대 성공!" << endl;
                cout << endl;
                cout << "+--------------------------+" << endl;
                cout << endl;
                break;
            }
        }

        if (i == Users.size()) {
            cout << endl;
            cout << "+----------WARNING---------+" << endl;
            cout << endl;
            cout << "          초대 실패" << endl;
            cout << endl;
            cout << "+--------------------------+" << endl;
            cout << endl;
        } else
            break;
    }

    // 기존에 있는 톡방인지 확인
    for (int i = 0; i < Rooms.size(); i++) {
        //  cout << "Rooms[i].getsend(): "<<Rooms[i].getSend() << "
        //  Rooms[i].getReceive() : "<<Rooms[i].getReceive() << endl;
        if (Rooms[i].getSend() == us &&
            Rooms[i].getReceive() == Users[me].getName()) {
            cout << "구성원이 동일한 채팅방이 존재합니다." << endl;
            return false;
        }
        if (Rooms[i].getReceive() == us &&
            Rooms[i].getSend() == Users[me].getName()) {
            cout << "구성원이 동일한 채팅방이 존재합니다." << endl;
            return false;
        }
    }

    Rooms.push_back(ChatRoom(ch, Users[me].getName(), us));
    chatOut();
    return true;
}

void signUp() {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "               Sign up               " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    download();
    string id = "";
    string pw = "";

    while (1) {
        cout << "ID (startMenu : q) : ";
        getline(cin, id);
        if (id == "q") {
            startMenu();
            return;
        }
        // 회원가입시 동일한 아이디가 있는지 확인
        int i;
        for (i = 0; i < Users.size(); i++) {
            if (Users[i].getName() == id) {
                cout << endl;
                cout << "+---------------WARNING--------------+" << endl;
                cout << endl;
                cout << "       동일한 아이디가 존재합니다." << endl;
                cout << endl;
                cout << "+------------------------------------+" << endl;
                cout << endl;
                break;
            }
        }

        if (i == Users.size())
            break;
    }

    // cout << "PW (input) : ";
    // getline(cin, pw);

    //비밀번호 보안
    char *p;
    p = getpass("PW (input) : ");
    pw = p;

    Users.push_back(User(id, pw));
    cout << "***회원가입 성공!***" << endl;
    upload();
    startMenu();
    return;
}

void signIn() {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "                Login                 " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    download();
    int tmp = 0;

    string id1 = "";
    string pw1 = "";

    vector<User>::iterator itr;
    int count = 0;

    while (tmp == 0) {
        string id2 = "";
        string pw2 = "";
        cout << "ID (startMenu : q): ";
        getline(cin, id2);
        if (id2 == "q") {
            startMenu();
            return;
        }
        // cout << "PW : ";
        // getline(cin, pw2)

        //비밀번호 보안
        char *p;
        p = getpass("PW : ");
        pw2 = p;

        /*for (itr = Users.begin(); itr != Users.end(); ++itr){
            cout << "name: " << (*itr).getName() << " PW: " <<
        (*itr).getPW() << endl;
        }
        */
        for (itr = Users.begin(); itr != Users.end(); ++itr) {
            if (id2 == (*itr).getName() && pw2 == (*itr).getPW()) {
                tmp = 1;
                break;
            }
            count++;
        }

        if (tmp == 0) {
            cout << endl;
            cout << "+--------------WARNING---------------+" << endl;
            cout << endl;
            cout << "    ID 혹은 PW 가 정확하지 않습니다!" << endl;
            cout << endl;
            cout << "+------------------------------------+" << endl;
            cout << endl;
        }
    }

    if (tmp == 1) {
        cout << "***로그인 성공!***" << endl;
        me = count;
        mainMenu();
        return;
    }
}

void startMenu() {
    while (1) {
        system("clear");
        cout << "+====================================+" << endl;
        cout << "             Ubuntu Talk              " << endl;
        cout << "+====================================+" << endl;
        cout << endl;
        cout << "채팅 프로그램에 오신 것을 환영합니다." << endl;
        cout << endl;
        cout << "0. 종료" << endl;
        cout << "1. 로그인" << endl;
        cout << "2. 회원가입" << endl;
        cout << endl;

        int choice = 0;
        cout << "Input: ";
        cin >> choice;
        cin.ignore(100, '\n');
        if (choice == 0) {
            out();
            return;
        }
        if (choice == 1) {
            signIn();
            return;
        }

        if (choice == 2) {
            signUp();
            return;
        }
    }
}

void mainMenu() {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "             Main Menu              " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    cout << Users[me].getName() << " 님, 안녕하세요!" << endl;

    while (1) {
        cout << endl;
        cout << "0. 종료" << endl;
        cout << "1. 상태메세지 보기" << endl;
        cout << "2. 나의 상태메세지 수정하기" << endl;
        cout << "3. 채팅 목록 열기" << endl;
        cout << endl;
        cout << "Input: ";

        int choice = 0;
        cin >> choice;
        cin.ignore(100, '\n');

        if (choice == 0) {
            out();
        }

        if (choice == 1) {
            seeStatus();
        }

        if (choice == 2) {
            changeStatus();
        }

        if (choice == 3) {
            chatList();
            return;
        }
    }
}

void seeStatus() {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "              상태메세지               " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    download();
    vector<User>::iterator it;
    for (it = Users.begin(); it != Users.end(); it++) {
        cout << (*it).getName();
        cout << "\t <[ " << (*it).getStatus() << " ]" << endl;
        cout << "--------------------------------------" << endl;
    }
    return;
}

void changeStatus() {
    system("clear");
    cout << "+====================================+" << endl;
    cout << "          상태메세지 수정하기          " << endl;
    cout << "+====================================+" << endl;
    cout << endl;
    download();
    cout << "현재 상태메세지 : "
         << "\t <[ " << Users[me].getStatus() << " ]" << endl;
    cout << endl;
    cout << "바꿀 상태메세지를 입력해주세요." << endl;
    cout << "Input: ";
    string temp;
    getline(cin, temp);
    Users[me].setStatus(temp);
    cout << endl;
    cout << "바뀐 상태메세지 : "
         << "\t <[ " << Users[me].getStatus() << " ]" << endl;
    upload();
    return;
}
