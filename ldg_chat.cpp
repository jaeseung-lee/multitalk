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
#define MAX_NAME_LENGTH 32
#define MAX_STATUS_sLENGTH 256

using namespace std;

class User {
  private:
    char name[MAX_NAME_LENGTH + 1];

  public:
    User(string newName);
    string getName();
};

void chatList();
void chatMake();
void setting();

vector<User> Users;

int main() {
    setting();
    chatList();
    return 0;
}

string User::getName(void) { return string(this->name); }

User::User(string newName) {
    memcpy(this->name, newName.c_str(), MAX_NAME_LENGTH + 1);
}

void setting() {
    Users.push_back(User("user1"));
    Users.push_back(User("user2"));
    Users.push_back(User("user3"));
    Users.push_back(User("user3"));
    vector<string> a = {"user2"};
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