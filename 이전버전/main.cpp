#include "chat.cpp"
#include <iostream>

int main(){
    signal(SIGINT,signalHandler);
    startMenu();
}
