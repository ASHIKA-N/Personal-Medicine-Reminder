#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include "Struct.hpp"
#include "FileHandler.hpp"

using namespace std;

class Stack
{
    vector<Action> a;

public:
    Stack(int s = 0)
    {
        if (s > 0)
            a.reserve(s);
    }

    void push(const Action &v) { a.push_back(v); }

    Action pop()
    {
        if (empty())
        {
            cout << "Stack Underflow\n";
            return Action();
        }
        Action v = a.back();
        a.pop_back();
        return v;
    }

    Action peek()
    {
        if (empty())
        {
            cout << "Stack Underflow\n";
            return Action();
        }
        return a.back();
    }

    bool empty() const { return a.empty(); }
    int size() const { return a.size(); }
    void clear() { a.clear(); }
};

class Queue
{
    vector<Med> a;

public:
    Queue(int s = 0)
    {
        if (s > 0)
            a.reserve(s);
    }

    void enqueue(const Med &v) { a.push_back(v); }

    Med dequeue()
    {
        if (empty())
        {
            cout << "Queue Underflow\n";
            return Med();
        }
        Med v = a.front();
        a.erase(a.begin());
        return v;
    }

    Med peek()
    {
        if (empty())
        {
            cout << "Queue Underflow\n";
            return Med();
        }
        return a.front();
    }

    bool empty() const { return a.empty(); }
    int size() const { return a.size(); }
};

struct PairHash
{
    size_t operator()(const pair<string, string> &p) const noexcept
    {
        return hash<string>()(p.first) ^ (hash<string>()(p.second) << 1);
    }
};

struct Node
{
    Med a;
    Node *next;
};

struct AlterRequest
{
    Node *node;
    bool changeName = false;
    bool changeDosage = false;
    bool changeQty = false;
    string newName;
    string newDosage;
    int newQty;
    bool changeTime = false;
    Time newTime;
    bool changeDays = false;
    vector<int> newDays;
    bool changeExp = false;
    Date newExp;
};

struct LinkedList
{
    Node *head;
    unordered_map<pair<string, string>, vector<Node *>, PairHash> hash;

    unordered_map<pair<string, string>, int, PairHash> qty;

    LinkedList() : head(nullptr) {}

    ~LinkedList()
    {
        Node *curr = head;
        while (curr)
        {
            Node *tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        hash.clear();
        qty.clear();
        head = nullptr;
    }

    void insert(Stack &u)
    {
        Action a;
        a.act = 'i';
        Node *t = new Node;

        cout << "Enter Name: ";
        getline(cin, t->a.name);
        cout << "Enter Dosage: ";
        getline(cin, t->a.dosage);
        int q;
        auto key = make_pair(t->a.name, t->a.dosage);

        if (qty.find(key) == qty.end())
        {
            cout << "Enter Quantity: ";
            cin >> q;
            qty[key] = q;
        }
        else
        {
            q = qty[key];
            cout << "Existing medicine detected. Using previous quantity (" << q << ").\n";
        }
        a.qb = q;
        cout << "Time (HH MM): ";
        cin >> t->a.t.h >> t->a.t.m;

        cout << "Expiry Date (DD MM YYYY): ";
        cin >> t->a.exp.d >> t->a.exp.m >> t->a.exp.y;

        int c;
        cout << "Which days to take?\n1.certain days\n2.daily\n";
        cin >> c;

        t->a.dy.clear();
        if (c == 1)
        {
            char o;
            cout << "It goes as 1-Monday ... 7-Sunday\n";
            for (int i = 1; i <= 7; i++)
            {
                cout << "Day " << i << "? (y/n): ";
                cin >> o;
                if (o == 'y' || o == 'Y')
                    t->a.dy.push_back(i);
            }
        }
        else
        {
            for (int i = 1; i <= 7; i++)
                t->a.dy.push_back(i);
        }

        t->next = nullptr;
        if (!head || t->a.t < head->a.t)
        {
            t->next = head;
            head = t;
        }
        else
        {
            Node *r = head;
            while (r->next && !(t->a.t < r->next->a.t))
                r = r->next;
            t->next = r->next;
            r->next = t;
        }

        hash[{t->a.name, t->a.dosage}].push_back(t);
        a.NV = t->a;
        u.push(a);
        cout << "Medicine added successfully!\n";
    }

    void disp()
    {
        if (!head)
        {
            cout << "No medicines in the list.\n";
            return;
        }
        Node *r = head;
        while (r)
        {
            r->a.disp();
            auto key = make_pair(r->a.name, r->a.dosage);
            if (qty.find(key) != qty.end())
                cout << "Quantity: " << qty[key] << endl;
            r = r->next;
        }
    }

    void del(const string &medName, const Time &t, Stack &u)
    {
        Action a;
        a.act = 'd';

        if (head->a.name == medName && t == head->a.t)
        {
            Node *temp = head;
            head = head->next;
            a.OV = temp->a;
            a.qb = qty[{temp->a.name, temp->a.dosage}];
            auto key = make_pair(temp->a.name, temp->a.dosage);
            auto &vec = hash[key];

            vec.erase(remove(vec.begin(), vec.end(), temp), vec.end());

            if (vec.empty())
            {
                hash.erase(key);
                qty.erase(key);
            }

            delete temp;
            cout << medName << " at ";
            t.disp();
            cout << " deleted.\n";
            u.push(a);
            return;
        }

        Node *r = head;
        while (r->next && !(r->next->a.name == medName && t == r->next->a.t))
            r = r->next;

        if (r->next)
        {
            Node *temp = r->next;
            r->next = temp->next;
            a.OV = temp->a;
            a.qb = qty[{temp->a.name, temp->a.dosage}];
            auto key = make_pair(temp->a.name, temp->a.dosage);
            auto &vec = hash[key];

            vec.erase(remove(vec.begin(), vec.end(), temp), vec.end());

            if (vec.empty())
            {
                hash.erase(key);
                qty.erase(key);
            }

            delete temp;
            cout << medName << " at ";
            t.disp();
            cout << " deleted.\n";
            u.push(a);
            return;
        }
        else
        {
            cout << medName << " at ";
            t.disp();
            cout << " not found.\n";
        }
    }

    void delAll(const string &medName, Stack &u)
    {
        Action c;
        c.act = 'a';
        bool found = false;

        while (head && head->a.name == medName)
        {
            found = true;
            Node *temp = head;
            head = head->next;
            c.OV = temp->a;
            c.qb = qty[{temp->a.name, temp->a.dosage}];
            u.push(c);
            auto key = make_pair(temp->a.name, temp->a.dosage);
            auto &vec = hash[key];

            vec.erase(remove(vec.begin(), vec.end(), temp), vec.end());

            if (vec.empty())
            {
                hash.erase(key);
                qty.erase(key);
            }

            delete temp;
        }

        Node *r = head;
        while (r && r->next)
        {
            if (r->next->a.name == medName)
            {
                found = true;
                Node *temp = r->next;
                r->next = temp->next;
                c.OV = temp->a;
                c.qb = qty[{temp->a.name, temp->a.dosage}];
                u.push(c);
                auto key = make_pair(temp->a.name, temp->a.dosage);
                auto &vec = hash[key];

                vec.erase(remove(vec.begin(), vec.end(), temp), vec.end());

                if (vec.empty())
                {
                    hash.erase(key);
                    qty.erase(key);
                }

                delete temp;
            }
            else
            {
                r = r->next;
            }
        }

        for (auto it = hash.begin(); it != hash.end();)
        {
            if (it->first.first == medName)
            {
                qty.erase(it->first);
                it = hash.erase(it);
            }
            else
                ++it;
        }

        if (found)
        {
            cout << "All medicines named \"" << medName << "\" deleted successfully.\n";
        }
        else
            cout << "No medicine named \"" << medName << "\" found.\n";
    }

    void search(const string &name)
    {
        bool found = false;

        for (auto &[key, vec] : hash)
        {
            if (key.first == name)
            {
                if (!found)
                {
                    cout << "Medicines named \"" << name << "\":\n";
                    cout << "----------------------------------\n";
                    found = true;
                }

                for (Node *n : vec)
                {
                    n->a.disp();
                    cout << "Quantity: " << qty[key] << endl;
                    cout << "----------------------------------\n";
                }
            }
        }

        if (!found)
            cout << "No medicine named \"" << name << "\" found.\n";
    }

    bool find(const string &name)
    {
        for (auto &p : hash)
            if (p.first.first == name)
                return true;
        return false;
    }

    void removeNode(Node *target)
    {
        if (!head || !target)
            return;

        Node *check = head;
        bool found = false;

        while (check)
        {
            if (check == target)
            {
                found = true;
                break;
            }
            check = check->next;
        }

        if (!found)
            return;

        if (head == target)
        {
            head = head->next;
            return;
        }

        Node *cur = head;
        while (cur->next && cur->next != target)
            cur = cur->next;

        if (cur->next == target)
            cur->next = target->next;
    }

    void insertSorted(Node *t)
    {
        if (!head || t->a.t < head->a.t)
        {
            t->next = head;
            head = t;
            return;
        }

        Node *cur = head;
        while (cur->next && !(t->a.t < cur->next->a.t))
            cur = cur->next;

        t->next = cur->next;
        cur->next = t;
    }

    void altermed(Stack &u)
    {
        if (hash.empty())
        {
            cout << "List is Empty\n";
            return;
        }

        Action c;
        c.act = 'u';

        string name, dosage;
        cout << "Enter medicine name to alter: ";
        getline(cin, name);
        cout << "Enter dosage to alter: ";
        getline(cin, dosage);

        auto key = make_pair(name, dosage);

        if (hash.find(key) == hash.end())
        {
            cout << "No such medicine found.\n";
            return;
        }

        auto &vec = hash[key];

        vector<AlterRequest> pending;

        for (Node *r : vec)
        {
            r->a.disp();
            cout << "Quantity: " << qty[key] << endl;

            char op;
            cout << "Do you want to change this entry? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (op != 'y' && op != 'Y')
                continue;

            AlterRequest req;
            req.node = r;

            cout << "Change medicine name? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (op == 'y' || op == 'Y')
            {
                cout << "Enter new name: ";
                getline(cin, req.newName);
                req.changeName = true;
            }

            cout << "Change time? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (op == 'y' || op == 'Y')
            {
                cout << "Enter hour and minute: ";
                cin >> req.newTime.h >> req.newTime.m;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                req.changeTime = true;
            }

            cout << "Change dosage? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (op == 'y' || op == 'Y')
            {
                cout << "Enter new dosage: ";
                getline(cin, req.newDosage);
                req.changeDosage = true;
            }

            cout << "Change days? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (op == 'y' || op == 'Y')
            {
                req.changeDays = true;
                req.newDays.clear();

                int choice;
                cout << "1. Certain days  2. Daily: ";
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (choice == 1)
                {
                    char o;
                    for (int i = 1; i <= 7; i++)
                    {
                        cout << "Day " << i << "? (y/n): ";
                        cin >> o;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (o == 'y' || o == 'Y')
                            req.newDays.push_back(i);
                    }
                }
                else
                {
                    for (int i = 1; i <= 7; i++)
                        req.newDays.push_back(i);
                }
            }

            cout << "Change quantity? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (op == 'y' || op == 'Y')
            {
                cout << "Enter new quantity: ";
                cin >> req.newQty;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                req.changeQty = true;
            }

            cout << "Change expiry date? (y/n): ";
            cin >> op;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (op == 'y' || op == 'Y')
            {
                int d, m, y;
                cout << "Day Month Year: ";
                cin >> d >> m >> y;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                req.newExp = Date(d, m, y);
                req.changeExp = true;
            }

            pending.push_back(req);
        }

        for (auto &req : pending)
        {
            Node *r = req.node;

            auto oldKey = make_pair(r->a.name, r->a.dosage);

            if (req.changeName)
                r->a.name = req.newName;

            if (req.changeDosage)
                r->a.dosage = req.newDosage;

            if (req.changeQty)
                qty[oldKey] = req.newQty;

            bool timeChanged = false;
            if (req.changeTime)
            {
                if (!(req.newTime == r->a.t))
                    timeChanged = true;

                r->a.t = req.newTime;
            }

            if (req.changeDays)
                r->a.dy = req.newDays;

            if (req.changeExp)
                r->a.exp = req.newExp;

            auto newKey = make_pair(r->a.name, r->a.dosage);

            if (timeChanged)
            {
                removeNode(r);
                r->next = nullptr;
                insertSorted(r);
                auto &vec2 = hash[newKey];

                sort(vec2.begin(), vec2.end(),
                     [](Node *a, Node *b)
                     {
                         return a->a.t < b->a.t;
                     });
            }

            if (newKey != oldKey)
            {
                int oldQuantity = qty[oldKey];
                if (req.changeQty)
                    qty[newKey] = req.newQty;
                else
                    qty[newKey] = oldQuantity;

                auto &oldVec = hash[oldKey];
                oldVec.erase(remove(oldVec.begin(), oldVec.end(), r), oldVec.end());

                if (oldVec.empty())
                {
                    hash.erase(oldKey);
                    qty.erase(oldKey);
                }

                hash[newKey].push_back(r);
            }
        }
    }

    void viewqty()
    {
        if (qty.empty())
        {
            cout << "No quantities recorded.\n";
            return;
        }

        cout << "\n--- Medicine Quantities ---\n";
        for (auto &[key, val] : qty)
        {
            cout << "Name: " << key.first
                 << " | Dosage: " << key.second
                 << " | Quantity: " << val << endl;
        }
        cout << "----------------------------\n";
    }

    int redqty(const string &name, const string &dosage)
    {
        auto key = make_pair(name, dosage);

        if (qty.find(key) == qty.end())
        {
            cout << "Medicine not found in quantity records.\n";
            return -1;
        }

        if (qty[key] == 0)
        {
            cout << "Medicine already depleted.\n";
            return 0;
        }

        qty[key]--;

        cout << "Medicine " << name
             << " (dosage: " << dosage
             << ") quantity reduced to " << qty[key] << endl;

        return qty[key];
    }

    void updqty(const string &name, const string &dosage)
    {
        if (qty.find({name, dosage}) == qty.end())
        {
            cout << "Medicine not found in quantity records. Cannot restock.\n";
            return;
        }
        cout << "Enter new quantity for " << name << " (dosage: " << dosage << "): ";
        cin >> qty[{name, dosage}];
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Quantity restocked to " << qty[{name, dosage}] << endl;
        return;
    }
};
