#include "../hppfolder/Login.hpp"
#include <filesystem>
#include <limits>
#include <algorithm>

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

bool Login::regist()
{
    std::filesystem::create_directories(LDATA_DIR);
    ofstream touch(file, ios::app);
    touch.close();
    string user, pass;
    int rq = 0;
    cout << "Enter Username: ";
    cin >> user;
    user = trim(user);
    transform(user.begin(), user.end(), user.begin(), ::tolower);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    ifstream filecheck(file);
    string u, v, r;

    while (filecheck >> u >> v >> r)
    {
        u = trim(u);
        transform(u.begin(), u.end(), u.begin(), ::tolower);

        if (u == user)
        {
            cout << "Username already exists! Try a different one.\n";
            return false;
        }
    }

    filecheck.close();

    cout << "Enter Password: ";
    cin >> pass;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string hashpass = hashPassword(pass);

    cout << "Remind me when any medicine quantity is <= (0 for only zero): ";
    if (!(cin >> rq) || rq < 0)
    {
        cout << "Invalid input. Using default reminder = 0\n";
        rq = 0;
        cin.clear();
    }

    ofstream filereg(file, ios::app);
    if (!filereg.is_open())
    {
        cout << "Could not open file!\n";
        return false;
    }

    filereg << user << " " << hashpass << " " << rq << endl;
    filereg.close();

    currentUser = user;
    cout << "Registered & logged in successfully!\n";
    return true;
}

bool Login::login()
{
    string user, pass;
    cout << "Enter Username: ";
    cin >> user;
    user = trim(user);
    transform(user.begin(), user.end(), user.begin(), ::tolower);
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

    string u, v, r;
    while (fileuser >> u >> v >> r)
    {
        if (u == user && v == hashpass)
        {
            cout << "Login successful!\n";
            try
            {
                remindQty = stoi(r);
            }
            catch (...)
            {
                remindQty = 0;
            }

            currentUser = user;
            fileuser.close();
            return true;
        }
    }

    fileuser.close();
    cout << "Invalid credentials!\n";
    return false;
}

void Login::changeRemindQty()
{
    int newQty;

    cout << "Current reminder threshold: " << remindQty << endl;
    cout << "Enter new reminder quantity (>= 0): ";
    cin >> newQty;

    if (!cin || newQty < 0)
    {
        cout << "Invalid value. Reminder not changed.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    remindQty = newQty;

    ifstream fin(file);
    ofstream fout(LDATA_DIR + "/temp.txt");

    if (!fin.is_open() || !fout.is_open())
    {
        cout << "Error updating reminder data.\n";
        return;
    }

    string u, v;
    int r;

    while (fin >> u >> v >> r)
    {
        if (u == currentUser)
            fout << u << " " << v << " " << remindQty << endl;
        else
            fout << u << " " << v << " " << r << endl;
    }

    fin.close();
    fout.close();

    remove(file.c_str());
    rename((LDATA_DIR + "/temp.txt").c_str(), file.c_str());

    cout << "Reminder updated successfully.\n";
}
