#include <iostream>
#include <limits>
#include "../hppfolder/FileHandler.hpp"
#include "../hppfolder/DS.hpp"
#include "../hppfolder/Expiry.hpp"
#include "../hppfolder/Queue.hpp"
#include "../hppfolder/Stack.hpp"
#include "../hppfolder/Login.hpp"
using namespace std;
// Main driver: handles authentication, menu flow, and session lifecycle

void menu()
{
    cout << "\n=== Personal Medication Reminder ===\n";
    cout << "Choices\n";
    cout << "1. View Medicine Schedule\n";
    cout << "2. Insert New Medicine\n";
    cout << "3. Delete a Medicine by Name and Time\n";
    cout << "4. Delete all Medicines of a Particular Name\n";
    cout << "5. Alter a Medicine\n";
    cout << "6. Search for a Medicine\n";
    cout << "7. Undo Last Operation\n";
    cout << "8. Redo Last Undone Operation\n";
    cout << "9. View Quantity\n";
    cout << "10. Exit\n";
}

int main()
{
    int ch, ch1;
    LinkedList L;
    Queue todayQueue;
    Stack undo, redo;
    Login log;
    int attempts = 0;
    bool login = false;

    while (attempts < 3 && !login)
    {
        if (attempts == 0)
        {
            cout << "1.Login\n2.Register\nChoice: ";
            cin >> ch1;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (ch1 == 1)
            {
                login = log.login();
            }
            else if (ch1 == 2)
            {
                login = log.regist();
            }
            else
            {
                cout << "Invalid option.\n";
                attempts++;
                continue;
            }
        }
        else
        {
            cout << "Login failed. Try again.\n";
            login = log.login();
        }

        attempts++;
    }
    if (!login)
    {
        cout << "Maximum login attempts exceeded. Exiting program.\n";
        return 0;
    }
    loadFromFile(L, log.getUser());

    while (true)
    {
        menu();
        cout << "Enter choice: ";
        cin >> ch;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (ch)
        {
        case 1:
            L.disp();
            break;

        case 2:
            if (!redo.empty())
                redo.clear();
            L.insert(undo);
            break;

        case 3:
        {
            if (!redo.empty())
                redo.clear();
            if (!L.head)
            {
                cout << "List is empty!\n";
                break;
            }
            string name;
            int h, m;
            cout << "Enter Name: ";
            getline(cin, name);
            if (!L.find(name))
            {
                cout << "No medicine found with that name.\n";
                break;
            }
            cout << "Enter Time (HH MM): ";
            cin >> h >> m;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            Time t(h, m);
            L.del(name, t, undo);
            break;
        }

        case 4:
        {
            if (!redo.empty())
                redo.clear();
            if (!L.head)
            {
                cout << "List is empty!\n";
                break;
            }
            string name;
            cout << "Enter Name: ";
            getline(cin, name);
            if (!L.find(name))
            {
                cout << "No medicine found with that name.\n";
                break;
            }
            L.delAll(name, undo);
            break;
        }

        case 5:
            if (!redo.empty())
                redo.clear();
            L.altermed(undo);
            break;

        case 6:
        {
            string name;
            cout << "Enter Name to Search: ";
            getline(cin, name);
            L.search(name);
            break;
        }

        case 7:
            Undo(undo, redo, L);
            break;

        case 8:
            Redo(redo, undo, L);
            break;
        case 9:
            L.viewqty();
            break;
        case 10:
        {
            int days;
            cout << "\n--- Expiry Check before Exit ---\n";
            cout << "Enter no. of days to remind before expiry: ";
            cin >> days;
            while (cin.fail() || days < 0)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Enter a non-negative number: ";
                cin >> days;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            expiry(&L, days);
            Queue todayQ = buildTodayQueue(L);
            reminderCheck(todayQ, L, log.getUser());
            commitToFile(L, log.getUser());
            cout << "Session Complete\nExiting\n";
            return 0;
        }
        default:
            cout << "Invalid Choice, Try Again\n";
        }
    }
}
