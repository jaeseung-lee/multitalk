#ifndef __Log_H__
#define __Log_H__

#include <iostream>
#include <string>

#define MAX_LEN 32

using namespace std;

class Person {
  public:
    Person();
    Person(string id, string pw);

    void setId(string id);
    void setPw(string pw);

    string getId(void);
    string getPw(void);

  private:
    char id[MAX_LEN + 1];
    char pw[MAX_LEN + 1];
};

#endif //__Log_H__
