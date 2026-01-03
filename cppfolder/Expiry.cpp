#include <iostream>
#include <chrono>
#include "../hppfolder/DS.hpp"
#include "../hppfolder/Expiry.hpp"

using namespace std;
using namespace std::chrono;

void expiry(LinkedList *L, int exp)
{
    if (!L || !L->head)
    {
        cout << "No medicines in list.\n";
        return;
    }

    Node *n = L->head;
    auto today = floor<days>(system_clock::now());
    year_month_day ymd{today};
    Date currentDate;
    currentDate.d = static_cast<unsigned>(ymd.day());
    currentDate.m = static_cast<unsigned>(ymd.month());
    currentDate.y = static_cast<int>(ymd.year());

    Stack dummy; // deletion during expiry should not affect undo history
    bool none = true;
    while (n)
    {
        char opt;
        Node *next = n->next;
        int diff = n->a.exp - currentDate;

        if (diff <= 0)
        {
            none = false;
            cout << "\nMedicine has expired\n";
            cout << "Name: " << n->a.name << " at ";
            n->a.t.disp();
            cout << endl;
            cout << "Do you need to refill (y/n): ";
            cin >> opt;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                opt = 'n';
            }
            if (opt == 'y' || opt == 'Y')
            {
                L->updqty(n->a.name, n->a.dosage);
                cout << "Enter new expiry date (dd mm yyyy): ";
                cin >> n->a.exp.d >> n->a.exp.m >> n->a.exp.y;
                cout << "Expiry date updated.\n";
            }
            else
            {
                cout << "No refill needed.\n";
                L->del(n->a.name, n->a.t, dummy);
            }
        }
        else if (diff <= exp)
        {
            none = false;
            cout << "\nMedicine will expire in " << diff << " day(s):\n";
            cout << "Name: " << n->a.name << " at ";

            n->a.t.disp();
            cout << endl;
            cout << "Do you need to refill (y/n): ";
            cin >> opt;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (opt == 'y' || opt == 'Y')
            {
                L->updqty(n->a.name, n->a.dosage);
            }
        }

        n = next;
    }
    if (none)
    {
        cout << "No medicines expiring within " << exp << " days.\n";
    }
    cout << "\nExpiry check complete.\n";
}