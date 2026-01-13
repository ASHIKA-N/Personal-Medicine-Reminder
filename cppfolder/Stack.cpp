#include <iostream>
#include <algorithm>
#include <limits> // FIX #7
#include "../hppfolder/Stack.hpp"

using namespace std;

void Undo(Stack &u, Stack &r, LinkedList &L)
{
    if (u.empty())
    {
        cout << "Nothing to undo.\n";
        return;
    }

    bool cont = true;
    char ch;

    while (cont)
    {
        Action a = u.peek();
        u.pop();

        // FIX #1 – preserve old quantity correctly
        int oldQty = a.qb;
        if (a.act == 'u')
        {
            auto key = make_pair(a.NV.name, a.NV.dosage);
            if (L.qty.count(key))
                a.qb = L.qty[key];
        }

        r.push(a);

        switch (a.act)
        {
        case 'i':
        {
            Node *curr = L.head, *prev = nullptr;
            while (curr && !(curr->a.name == a.NV.name && curr->a.t == a.NV.t))
            {
                prev = curr;
                curr = curr->next;
            }
            if (!curr)
                break;

            if (!prev)
                L.head = curr->next;
            else
                prev->next = curr->next;

            auto key = make_pair(curr->a.name, curr->a.dosage);

            // FIX #6 – ensure hash removal only if present
            if (L.hash.count(key))
            {
                auto &vec = L.hash[key];
                vec.erase(remove(vec.begin(), vec.end(), curr), vec.end());
                if (vec.empty())
                    L.hash.erase(key);
            }

            bool stillExists = false;
            for (Node *run = L.head; run; run = run->next)
            {
                if (run->a.name == curr->a.name && run->a.dosage == curr->a.dosage)
                {
                    stillExists = true;
                    break;
                }
            }

            if (!stillExists)
                L.qty.erase(key);

            delete curr;
            cout << "Undo insert -> Deleted: " << a.NV.name << " at ";
            a.NV.t.disp();
            cout << endl;
            break;
        }

        case 'd':
        case 'a':
        {
            Node *newNode = new Node;
            newNode->a = a.OV;
            newNode->next = nullptr;

            if (!L.head || newNode->a.t < L.head->a.t)
            {
                newNode->next = L.head;
                L.head = newNode;
            }
            else
            {
                Node *curr = L.head;
                while (curr->next && !(newNode->a.t < curr->next->a.t))
                    curr = curr->next;
                newNode->next = curr->next;
                curr->next = newNode;
            }

            auto key = make_pair(newNode->a.name, newNode->a.dosage);
            L.hash[key].push_back(newNode);
            L.qty[key] = oldQty;

            cout << "Undo delete -> Restored: " << newNode->a.name << " at ";
            newNode->a.t.disp();
            cout << endl;
            break;
        }

        case 'u':
        {
            Node *curr = L.head;
            while (curr)
            {
                if (curr->a.name == a.NV.name && curr->a.t == a.NV.t)
                {
                    auto oldKey = make_pair(a.NV.name, a.NV.dosage);
                    auto newKey = make_pair(a.OV.name, a.OV.dosage);

                    if (oldKey != newKey && L.hash.count(oldKey))
                    {
                        auto &vec = L.hash[oldKey];
                        vec.erase(remove(vec.begin(), vec.end(), curr), vec.end());
                        if (vec.empty())
                            L.hash.erase(oldKey);

                        L.hash[newKey].push_back(curr);
                    }

                    L.qty[newKey] = oldQty;
                    L.qty.erase(oldKey);

                    curr->a = a.OV;
                    break;
                }
                curr = curr->next;
            }
            cout << "Undo update -> Reverted last alteration.\n";
            break;
        }
        }

        if (u.empty())
        {
            cout << "Nothing left to Undo\n";
            return;
        }

        cout << "Continue undoing? (y/n): ";
        cin >> ch;

        // FIX #7 – input safety
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        if (ch != 'y' && ch != 'Y')
            cont = false;
    }
}

void Redo(Stack &r, Stack &u, LinkedList &L)
{
    if (r.empty())
    {
        cout << "Nothing to redo.\n";
        return;
    }

    bool cont = true;
    char ch;

    while (cont)
    {
        Action a = r.peek();
        r.pop();

        int oldQty = a.qb;
        if (a.act == 'u')
        {
            auto key = make_pair(a.NV.name, a.NV.dosage);
            if (L.qty.count(key))
                a.qb = L.qty[key];
        }

        u.push(a);

        switch (a.act)
        {
        case 'i':
        {
            Node *newNode = new Node;
            newNode->a = a.NV;
            newNode->next = nullptr;

            if (!L.head || newNode->a.t < L.head->a.t)
            {
                newNode->next = L.head;
                L.head = newNode;
            }
            else
            {
                Node *curr = L.head;
                while (curr->next && !(newNode->a.t < curr->next->a.t))
                    curr = curr->next;
                newNode->next = curr->next;
                curr->next = newNode;
            }

            auto key = make_pair(newNode->a.name, newNode->a.dosage);
            L.hash[key].push_back(newNode);
            L.qty[key] = oldQty;

            cout << "Redo insert -> Inserted: " << newNode->a.name << " at ";
            newNode->a.t.disp();
            cout << endl;
            break;
        }

        case 'd':
        case 'a':
        {
            Node *curr = L.head, *prev = nullptr;
            while (curr && !(curr->a.name == a.OV.name && curr->a.t == a.OV.t))
            {
                prev = curr;
                curr = curr->next;
            }

            if (!curr)
            {
                // FIX #5 – no continue, safe exit
                cout << "Redo skipped: item no longer exists.\n";
                break;
            }

            if (!prev)
                L.head = curr->next;
            else
                prev->next = curr->next;

            auto key = make_pair(curr->a.name, curr->a.dosage);

            if (L.hash.count(key))
            {
                auto &vec = L.hash[key];
                vec.erase(remove(vec.begin(), vec.end(), curr), vec.end());
                if (vec.empty())
                    L.hash.erase(key);
            }

            L.qty.erase(key);

            delete curr;
            cout << "Redo delete -> Deleted.\n";
            break;
        }

        case 'u':
        {
            Node *curr = L.head;
            while (curr)
            {
                if (curr->a.name == a.OV.name && curr->a.t == a.OV.t)
                {
                    auto oldKey = make_pair(a.OV.name, a.OV.dosage);
                    auto newKey = make_pair(a.NV.name, a.NV.dosage);

                    if (oldKey != newKey && L.hash.count(oldKey))
                    {
                        auto &vec = L.hash[oldKey];
                        vec.erase(remove(vec.begin(), vec.end(), curr), vec.end());
                        if (vec.empty())
                            L.hash.erase(oldKey);

                        L.hash[newKey].push_back(curr);
                    }

                    L.qty[newKey] = oldQty;
                    L.qty.erase(oldKey);

                    curr->a = a.NV;
                    break;
                }
                curr = curr->next;
            }

            cout << "Redo update -> Reapplied alteration.\n";
            break;
        }
        }

        if (r.empty())
        {
            cout << "Nothing left to Redo\n";
            return;
        }

        cout << "Continue redoing? (y/n): ";
        cin >> ch;

        // FIX #7 – input safety
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        if (ch != 'y' && ch != 'Y')
            cont = false;
    }
}
