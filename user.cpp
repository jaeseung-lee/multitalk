#include "chat.cpp"
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
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
