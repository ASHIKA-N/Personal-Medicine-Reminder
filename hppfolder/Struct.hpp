#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iomanip>
using namespace std;
using namespace std::chrono;

struct Time
{
    int h, m;

    Time(int hour = 0, int min = 0)
    {
        h = (hour >= 0 && hour < 24) ? hour : 0;
        m = (min >= 0 && min < 60) ? min : 0;
    }

    int toMinutes() const { return h * 60 + m; }

    void disp() const
    {
        cout << setfill('0') << setw(2) << h << ":"
             << setfill('0') << setw(2) << m;
    }

    bool operator<(const Time &other) const { return toMinutes() < other.toMinutes(); }
    bool operator<=(const Time &other) const { return toMinutes() <= other.toMinutes(); }
    bool operator==(const Time &other) const { return toMinutes() == other.toMinutes(); }
};

struct Date
{
    int d, m, y;

    Date(int day = 1, int month = 1, int year = 2000)
    {
        if (isValid(day, month, year))
        {
            d = day;
            m = month;
            y = year;
        }
        else
        {
            d = 1;
            m = 1;
            y = 2000;
        }
    }

    static bool isValid(int day, int month, int year)
    {
        if (year < 1 || month < 1 || month > 12)
            return false;
        int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool leap = (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);
        if (leap)
            daysInMonth[2] = 29;
        return day >= 1 && day <= daysInMonth[month];
    }

    void print() const
    {
        cout << setfill('0')
             << setw(2) << d << "/"
             << setw(2) << m << "/"
             << setw(4) << y;
    }

    int operator-(const Date &dt) const
    {
        sys_days thisDate = year{y} / month{static_cast<unsigned>(m)} / day{static_cast<unsigned>(d)};
        sys_days otherDate = year{dt.y} / month{static_cast<unsigned>(dt.m)} / day{static_cast<unsigned>(dt.d)};
        return (thisDate - otherDate).count();
    }

    bool operator<=(const Date &other) const
    {
        sys_days thisDate = year{y} / month{static_cast<unsigned>(m)} / day{static_cast<unsigned>(d)};
        sys_days otherDate = year{other.y} / month{static_cast<unsigned>(other.m)} / day{static_cast<unsigned>(other.d)};
        return thisDate <= otherDate;
    }
};

struct Med
{
    string name;
    string dosage;
    Time t;
    Date exp;
    vector<int> dy;

    Med() : name(""), dosage(""), t(0, 0), exp(1, 1, 2000), dy({}) {}

    Med(const string &n, const string &d, int hour, int min, vector<int> f, int day, int month, int year)
    {
        name = n;
        dosage = d;
        t = Time(hour, min);
        dy = f.empty() ? vector<int>{1, 2, 3, 4, 5, 6, 7} : f;
        if (Date::isValid(day, month, year))
            exp = Date(day, month, year);
        else
            exp = Date(1, 1, 2000);
    }

    void disp() const
    {
        cout << "\n-----------------\n";
        cout << "Name: " << name << "\nDosage: " << dosage << "\nIntake Time: ";
        t.disp();
        cout << "\nDays to consume: ";
        for (int d : dy)
            cout << d << " ";
        cout << "\nExpiry Date: ";
        exp.print();
        cout << "\n-----------------\n";
    }

    bool operator==(const Med &other) const
    {
        return name == other.name && t == other.t && dosage == other.dosage;
    }
};

struct Action
{
    char act;
    Med OV;
    Med NV;
    int qb;

    Action() : act('n'), OV(Med()), NV(Med()) {}
};
