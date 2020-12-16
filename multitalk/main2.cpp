#include "chat2.cpp"
#include <iostream>

int main(){
    signal(SIGINT,signalHandler);
    startMenu();
}
