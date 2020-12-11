#include "chat.cpp"
#include <vector>
#include <iostream>

using namespace std;

//현재 접속한 유저의 정보
User now("user1");
vector<string> whoIsReceiver;

void setting();
void _chatList();
void _chatRoom(vector<string> receiver);

int main(){
    setting();
    _chatList();
    return 0;
}

void setting(){
    Users.push_back(User("user1"));
    Users.push_back(User("user2"));
    Users.push_back(User("user3"));
    Users.push_back(User("user3"));
    vector<string> a={"user2"};
    Chats.push_back(Chat("user1",a,"hi","time"));
    cout << "User: " << Users.size() << " Chats:"  << Chats.size() << endl;

    vector<Chat>::iterator xx;
    for(xx=Chats.begin();xx!=Chats.end();xx++)
        cout << (*xx).getChatting() << endl;
}

void _chatList(){
    vector<User>::iterator uit;
    while(1){
        //메뉴 표시
        cout << "chatlist:" << endl;
        int count=0;
        cout << "0. back" << endl;

        for(uit=Users.begin();uit!=Users.end();uit++){
            if((*uit).getName()==now.getName())
                continue;
            count++;
            cout << count << ". " << (*uit).getName() << endl;
        }
        count++;
        cout << count << ". Make Group Chatting Room." << endl;
        cout << "<<";

        //번호 입력받음
        int choice;
        cin >> choice;
        if(choice==0){
            cout << "mainmenu()"<<endl;
            //mainMenu();
            return ;
        }
        else if(choice==count)
            chatOut();

        else if(choice<count){
            //누가 보냈는지 알아내는 과정
            int temp=0;
            whoIsReceiver.clear();
            for(uit=Users.begin();uit!=Users.end();uit++){
                if((*uit).getName()==now.getName())
                    continue;
                temp++;
                if(temp==count){
                    whoIsReceiver.push_back((*uit).getName());
                    break;
                }
            }
        _chatRoom(whoIsReceiver);
        return;
        }
    }

}

void _chatRoom(vector<string> receiver){
    vector<string> me;
    me.push_back(now.getName());

    while(1){
        //fflush(stdin);
        cout << "Chatting Room : with ";
        for(int i=0;i<receiver.size();i++)
            cout << receiver[i];

        cout << endl;
        //Chat에서 receive 가 receiver하고 동일한 내용을 다 찾아와서 순서대로 표시한다.
        //받은 메세지는 "<<(내용)" 형태로, 보낸 메세지는 ">>(내용)" 형태로 표시한다.
        vector<Chat>::iterator cit;

        for(cit=Chats.begin();cit!=Chats.end();cit++){
            // 보낸 사람이 나, 받은사람이 상대방
            if((*cit).getReceive()==receiver && (*cit).getSend()==now.getName())
                cout << (*cit).getTime() << " << " << (*cit).getChatting() << endl;

            // 받은 사람이 나, 보낸 사람이 상대방 -> 받은 사람이 나 이므로 vector로 표현 -> 그게 위에 정의한 vector<string> me
            if((*cit).getReceive()==me){
                vector<string>::iterator rit;
                // receive vector안에 받은사람이 내가 있는지 확인.
                for(rit=(*cit).getReceive().begin();rit!=(*cit).getReceive().end();rit++){
                    if((*rit)==now.getName()) {
                        cout << (*cit).getTime() << " >> " << (*cit).getChatting() << endl;
                        break;
                    }
                }
            }
        }
        cout << "chat size: "<< Chats.size() << endl;
        cout << ":";
        string tempchat;
        getline(cin,tempchat);

        if(tempchat=="b")
            _chatList();
        Chats.push_back(Chat(now.getName(),receiver,tempchat,"time"));
        }
}
