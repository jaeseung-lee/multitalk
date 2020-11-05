#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>


using namespace std;

class User{
private:
	string name;
	string status;
private:
	void setStatus(string newStatus);
	void getName();
	void getStatus();
};

struct __Chat{
	string send;
	vector<string> receive;
	string chatting;
	string times;
};

typedef struct __Chat chat;

vector<chat> Chats;

void download();
void upload();
void startMenu();
void signUp();
void signIn();
void mainMenu();
void seeStatus();
void changeStatus();
void chatList();
void chatting();
void out();

int main(int args, char* argv[]){
}
