#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
static const string LDATA_DIR = "login_data";
using namespace std;
class Login
{
private:
    string file = LDATA_DIR + "/" + "user.txt";
    string currentUser = "";
    string hashPassword(const string &password);
    static string trim(const string &s);

public:
    void regist();
    bool login();
    string getUser() const { return currentUser; }
};
