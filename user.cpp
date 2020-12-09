#include "chat.cpp"
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;
#define PERMS 0664
typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    char status[MAX_STATUS_LENGTH + 1];
} Userinfo;

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

void chatOut() {
    int fd;
    fd = open("/tmp/chats.txt", O_CREAT | O_APPEND | O_WRONLY, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }

    // chat vector를 클리어해주고, 다시 전부 담는다.
    Chats.clear();

    Chat temp_chat;
    memset(temp_chat.send, 0x00, MAX_NAME_LENGTH);
    memset(temp_chat.chatting, 0x00, MAX_CHAT_LENGTH);
    temp_chat.receive.clear();
    memset(temp_chat.time, 0x00, MAX_NAME_LENGTH);

    size_t r_size = 0;
    while (1) {
        r_size = read(fd, (Chat *)&temp_chat, sizeof(temp_chat));
        if (r_size == -1)
            break;

        Chats.push_back(temp_chat);
    }
}

void chatIn() {
    int fd;
    //파일을 지우고 새로 다시 다 담는다.
    remove("tmp/chats.txt");
    fd = open("/tmp/chats.txt", O_CREAT | O_APPEND | O_WRONLY, PERMS);
    if (fd == -1) {
        perror("open error!");
        exit(-1);
    }
    size_t w_size = 0;
    Chat temp_chat;
    memset(temp_chat.send, 0x00, MAX_NAME_LENGTH);
    memset(temp_chat.chatting, 0x00, MAX_CHAT_LENGTH);
    temp_chat.receive.clear();
    memset(temp_chat.time, 0x00, MAX_NAME_LENGTH);

    for (int i = 0; i < Chats.size(); i++) {
        strcpy(temp_chat.send, Chats[i].send);
        strcpy(temp_chat.chatting, Chats[i].chatting);
        strcpy(temp_chat.time, Chats[i].time);
        temp_chat.receive = Chats[i].receive;

        if ((w_size = write(fd, (Chat *)&temp_chat, sizeof(Chat))) == -1) {
            perror("write() error!");
            exit(-1);
        }
    }
}
