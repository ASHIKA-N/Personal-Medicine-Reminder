#include "../hppfolder/Login.hpp"
#include <limits>

string Login::trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string Login::hashPassword(const string &password)
{
    hash<string> hashing;
    return to_string(hashing(password));
}

void Login::regist()
{

    string user, pass;
    cout << "Enter Username: ";
    cin >> user;
    user = trim(user);
    cout << "Enter Password: ";
    cin >> pass;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string hashpass = hashPassword(pass);

    ifstream filecheck(file);
    if (!filecheck.is_open())
    {
        cout << "Error: Could not open user file for reading.\n";
        return;
    }
    string u, v;
    while (filecheck >> u >> v)
    {
        if (u == user)
        {
            cout << "Username already exists! Try a different one.\n";
            filecheck.close();
            return;
        }
    }
    filecheck.close();

    ofstream filereg(file, ios::app);
    if (filereg.is_open())
    {
        filereg << user << " " << hashpass << endl;
        cout << "Registered successfully!\n";
        filereg.close();
    }
    else
    {
        cout << "Could not open file!\n";
    }
}

bool Login::login()
{
    string user, pass;
    cout << "Enter Username: ";
    cin >> user;
    user = trim(user);
    cout << "Enter Password: ";
    cin >> pass;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string hashpass = hashPassword(pass);

    ifstream fileuser(file);
    if (!fileuser.is_open())
    {
        cout << "No users registered yet. Please register first.\n";
        return false;
    }

    fileuser.seekg(0, ios::end);
    if (fileuser.tellg() == 0)
    {
        cout << "No users registered yet. Please register first.\n";
        return false;
    }
    fileuser.seekg(0, ios::beg);

    string u, v;
    while (fileuser >> u >> v)
    {
        if (u == user && v == hashpass)
        {
            cout << "Login successful!\n";
            currentUser = user;
            fileuser.close();
            return true;
        }
    }

    fileuser.close();
    cout << "Invalid credentials!\n";
    return false;
}
