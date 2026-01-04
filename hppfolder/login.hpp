#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
using namespace std;
static const string LDATA_DIR = "login_data";
class Login
{
private:
    string file = LDATA_DIR + "/" + "user.txt";
    string currentUser = "";
    string hashPassword(const string &password);
    static string trim(const string &s);
    int remindQty;

public:
    bool regist();
    bool login();
    string getUser() const { return currentUser; }
    int getRemindQty() const { return remindQty; }
    void changeRemindQty();
};
