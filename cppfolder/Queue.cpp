#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <limits>
#include <algorithm>
#include <fstream>
#include "../hppfolder/Queue.hpp"

using namespace std;
static const string MDATA_DIR = "missed_data";

int getTodayDayNumber()
{
    using namespace std::chrono;
    weekday wd{floor<days>(system_clock::now())};
    return wd.c_encoding() == 0 ? 7 : wd.c_encoding();
}

Queue buildTodayQueue(const LinkedList &L)
{
    Queue todayQ;
    if (!L.head)
    {
        cout << "No medicines in schedule.\n";
        return todayQ;
    }

    int today = getTodayDayNumber();
    cout << "\nToday is day number: " << today << " (1=Mon..7=Sun)\n";

    Node *r = L.head;
    while (r)
    {
        if (find(r->a.dy.begin(), r->a.dy.end(), today) != r->a.dy.end())
        {
            todayQ.enqueue(r->a);
        }
        r = r->next;
    }

    cout << "No of medicines to take today: " << todayQ.size() << "\n";
    return todayQ;
}

bool exitRequested()
{
    if (_kbhit())
    {
        char c = _getch();
        if (c == 'e' || c == 'E')
            return true;
    }
    return false;
}

void reminderCheck(Queue &todayQ, LinkedList &L, const string &username, int remindQty)
{
    bool eFlag = false;
    vector<Med> missed;

    string logFileName = MDATA_DIR + "/" + username + "_missed_log.txt";

    ofstream logFile(logFileName, ios::app);
    if (!logFile)
    {
        cerr << "Error opening missed_log.txt\n";
        return;
    }

    time_t t = time(0);
    logFile << "\n--- Missed Medicines on " << ctime(&t) << "---\n";

    while (!todayQ.empty() && !eFlag)
    {
        Med m = todayQ.peek();

        auto now = chrono::system_clock::now();
        time_t t_now = chrono::system_clock::to_time_t(now);
        tm *ltm = localtime(&t_now);
        int currHour = ltm->tm_hour;
        int currMin = ltm->tm_min;

        if (currHour > m.t.h || (currHour == m.t.h && currMin >= m.t.m))
        {
            cout << "\nReminder: Time to take medicine " << m.name
                 << " (" << m.dosage << ") at ";
            m.t.disp();

            cout << "\nYou can exit anytime by pressing 'e'.\n";
            char taken;
            while (true)
            {
                cout << "Mark as taken (y/n) or 'e' to exit: ";
                cin >> taken;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                taken = tolower(taken);
                if (taken == 'y' || taken == 'n' || taken == 'e')
                    break;
                cout << "Invalid choice. Please enter y, n, or e.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            if (taken == 'y')
            {
                int remaining = L.redqty(m.name, m.dosage);
                todayQ.dequeue();
                cout << "Medicine marked as taken.\n";

                if (remaining == -1)
                    cout << "Error: quantity record missing. Skipping stock check.\n";

                if (remaining <= remindQty)
                {
                    cout << "\n[STOCK REMINDER]\n";
                    cout << "Medicine: " << m.name << " (" << m.dosage << ")\n";
                    cout << "Remaining Quantity: " << remaining << endl;
                    cout << "Reminder Threshold: " << remindQty << endl;

                    if (remaining == 0)
                    {
                        char ch;
                        cout << "Stock depleted. Would you like to restock now? (y/n): ";
                        cin >> ch;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        if (tolower(ch) == 'y')
                            L.updqty(m.name, m.dosage);
                    }
                }
            }
            else if (taken == 'e')
            {
                eFlag = true;
                break;
            }
            else
            {
                int skipCount = 0;
                cout << "Snoozed! Will remind again after 20 minutes. Press 'e' anytime to exit.\n";
                for (int minute = 0; minute < 20 && !eFlag && skipCount < 3; ++minute)
                {
                    for (int sec = 0; sec < 60; ++sec)
                    {
                        if (exitRequested())
                        {
                            cout << "\nExiting snooze early.\n";
                            eFlag = true;
                            break;
                        }
                        this_thread::sleep_for(chrono::seconds(1));
                    }
                    if (!eFlag)
                        cout << (20 - minute - 1) << " minutes left...\n";
                    ++skipCount;
                }

                if (eFlag)
                    break;

                missed.push_back(m);
                todayQ.dequeue();
                cout << "Medicine marked as missed.\n";
                logFile << m.name << " | " << m.dosage << " | ";
                logFile << (m.t.h < 10 ? "0" : "") << m.t.h << ":"
                        << (m.t.m < 10 ? "0" : "") << m.t.m << "\n";

                continue;
            }
        }
        else
        {
            cout << "\nNext medicine (" << m.name << ") at ";
            m.t.disp();
            cout << "\n(Press 'e' anytime to exit.)\n";

            for (int i = 0; i < 30 && !eFlag; ++i)
            {
                if (exitRequested())
                {
                    cout << "\nExit requested.\n";
                    eFlag = true;
                    break;
                }
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    }

    if (eFlag)
    {
        while (!todayQ.empty())
        {
            missed.push_back(todayQ.dequeue());
        }

        for (const auto &m : missed)
        {
            logFile << m.name << " | " << m.dosage << " | ";
            logFile << (m.t.h < 10 ? "0" : "") << m.t.h << ":"
                    << (m.t.m < 10 ? "0" : "") << m.t.m << "\n";
        }
    }

    if (!eFlag)
        cout << "\nAll today's medicines are taken!\n";
    else
        cout << "\nSession ended. " << todayQ.size()
             << " medicine(s) still remaining today.\n";
    logFile << "--- End of Missed Medicines ---\n";
    logFile.close();
}