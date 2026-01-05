#include <iostream>
#include <algorithm>
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
        int oldQty;
        Action a = u.peek();
        u.pop();
        if (a.act == 'u')
        {
            oldQty = a.qb;
            a.qb = L.qty[{a.NV.name, a.NV.dosage}];
        }
        r.push(a);

        switch (a.act)
        {

        case 'i':
        {
            string name = a.NV.name;
            string dosage = a.NV.dosage;
            Time t = a.NV.t;

            Node *curr = L.head, *prev = nullptr;
            while (curr && !(curr->a.name == name && curr->a.t == t))
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

            auto &vec = L.hash[{name, dosage}];
            vec.erase(remove(vec.begin(), vec.end(), curr), vec.end());
            if (vec.empty())
                L.hash.erase({name, dosage});

            auto key = make_pair(curr->a.name, curr->a.dosage);

            bool stillExists = false;
            Node *run = L.head;
            while (run)
            {
                if (run->a.name == curr->a.name && run->a.dosage == curr->a.dosage)
                {
                    stillExists = true;
                    break;
                }
                run = run->next;
            }

            if (!stillExists)
                L.qty.erase(key);

            delete curr;
            cout << "Undo insert -> Deleted: " << name << " at ";
            t.disp();
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

            L.hash[{newNode->a.name, newNode->a.dosage}].push_back(newNode);
            L.qty[{newNode->a.name, newNode->a.dosage}] = a.qb;
            cout << "Undo delete -> Restored: " << newNode->a.name << " at ";
            newNode->a.t.disp();
            cout << endl;

            if (!u.empty() && u.peek().act == 'a')
            {
                char o;
                cout << "More deleted items detected. Restore all? (y for all): ";
                cin >> o;
                if (o == 'y' || o == 'Y')
                {
                    while (!u.empty() && u.peek().act == 'a')
                    {
                        Action x = u.peek();
                        u.pop();
                        r.push(x);

                        Node *newNode2 = new Node;
                        newNode2->a = x.OV;
                        newNode2->next = nullptr;

                        if (!L.head || newNode2->a.t < L.head->a.t)
                        {
                            newNode2->next = L.head;
                            L.head = newNode2;
                        }
                        else
                        {
                            Node *curr = L.head;
                            while (curr->next && !(newNode2->a.t < curr->next->a.t))
                                curr = curr->next;
                            newNode2->next = curr->next;
                            curr->next = newNode2;
                        }

                        L.hash[{newNode2->a.name, newNode2->a.dosage}].push_back(newNode2);
                        L.qty[{newNode2->a.name, newNode2->a.dosage}] = x.qb;
                    }
                    cout << "All deleted items restored.\n";
                }
            }
            break;
        }

        case 'u':
        {
            Node *curr = L.head;
            while (curr)
            {
                if (curr->a.name == a.NV.name && curr->a.t == a.NV.t)
                {
                    if (a.NV.name != a.OV.name || a.NV.dosage != a.OV.dosage)
                    {
                        auto oldKey = make_pair(a.NV.name, a.NV.dosage);
                        auto newKey = make_pair(a.OV.name, a.OV.dosage);

                        auto &oldVec = L.hash[oldKey];
                        oldVec.erase(remove(oldVec.begin(), oldVec.end(), curr), oldVec.end());
                        if (oldVec.empty())
                            L.hash.erase(oldKey);

                        L.hash[newKey].push_back(curr);
                    }

                    auto oldKey = make_pair(a.OV.name, a.OV.dosage);
                    auto newKey = make_pair(a.NV.name, a.NV.dosage);

                    int oldQtyVal = L.qty.count(newKey) ? L.qty[newKey] : a.qb;
                    L.qty[oldKey] = oldQtyVal;

                    bool stillUsed = false;
                    for (Node *tmp = L.head; tmp; tmp = tmp->next)
                    {
                        if (tmp != curr && tmp->a.name == a.NV.name && tmp->a.dosage == a.NV.dosage)
                        {
                            stillUsed = true;
                            break;
                        }
                    }
                    if (!stillUsed)
                        L.qty.erase(newKey);

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
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
        int oldQty;
        Action a = r.peek();
        r.pop();
        if (a.act == 'u')
        {
            oldQty = a.qb;
            a.qb = L.qty[{a.NV.name, a.NV.dosage}];
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

            L.hash[{newNode->a.name, newNode->a.dosage}].push_back(newNode);
            L.qty[{newNode->a.name, newNode->a.dosage}] = a.qb;
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
                break;

            if (!prev)
                L.head = curr->next;
            else
                prev->next = curr->next;

            auto &vec = L.hash[{curr->a.name, curr->a.dosage}];
            vec.erase(remove(vec.begin(), vec.end(), curr), vec.end());
            if (vec.empty())
                L.hash.erase({curr->a.name, curr->a.dosage});

            auto key = make_pair(curr->a.name, curr->a.dosage);
            bool stillExists = false;
            Node *run = L.head;
            while (run)
            {
                if (run->a.name == curr->a.name && run->a.dosage == curr->a.dosage)
                {
                    stillExists = true;
                    break;
                }
                run = run->next;
            }

            if (!stillExists)
                L.qty.erase(key);

            cout << "Redo delete -> Deleted: " << curr->a.name << " at ";
            curr->a.t.disp();
            cout << endl;
            delete curr;

            if (!r.empty() && r.peek().act == 'a')
            {
                char o;
                cout << "More deletions detected. Redo all? (y for all): ";
                cin >> o;
                if (o == 'y' || o == 'Y')
                {
                    while (!r.empty() && r.peek().act == 'a')
                    {
                        Action x = r.peek();
                        r.pop();
                        u.push(x);

                        Node *curr2 = L.head, *prev2 = nullptr;
                        while (curr2 && !(curr2->a.name == x.OV.name && curr2->a.t == x.OV.t))
                        {
                            prev2 = curr2;
                            curr2 = curr2->next;
                        }
                        if (!curr2)
                        {
                            cout << "Redo skipped: item no longer exists.\n";
                            continue;
                        }

                        if (!prev2)
                            L.head = curr2->next;
                        else
                            prev2->next = curr2->next;

                        auto &vec2 = L.hash[{curr2->a.name, curr2->a.dosage}];
                        vec2.erase(remove(vec2.begin(), vec2.end(), curr2), vec2.end());
                        if (vec2.empty())
                            L.hash.erase({curr2->a.name, curr2->a.dosage});

                        auto key = make_pair(curr2->a.name, curr2->a.dosage);
                        stillExists = false;
                        run = L.head;
                        while (run)
                        {
                            if (run->a.name == curr2->a.name && run->a.dosage == curr2->a.dosage)
                            {
                                stillExists = true;
                                break;
                            }
                            run = run->next;
                        }

                        if (!stillExists)
                            L.qty.erase(key);

                        delete curr2;
                    }
                    cout << "All group deletions redone.\n";
                }
            }
            break;
        }

        case 'u':
        {
            Node *curr = L.head;
            while (curr)
            {
                if (curr->a.name == a.OV.name && curr->a.t == a.OV.t)
                {
                    if (a.OV.name != a.NV.name || a.OV.dosage != a.NV.dosage)
                    {
                        auto oldKey = make_pair(a.OV.name, a.OV.dosage);
                        auto newKey = make_pair(a.NV.name, a.NV.dosage);

                        auto &oldVec = L.hash[oldKey];
                        oldVec.erase(remove(oldVec.begin(), oldVec.end(), curr), oldVec.end());
                        if (oldVec.empty())
                            L.hash.erase(oldKey);

                        L.hash[newKey].push_back(curr);
                    }

                    auto oldKey = make_pair(a.OV.name, a.OV.dosage);
                    auto newKey = make_pair(a.NV.name, a.NV.dosage);

                    int oldQtyVal = L.qty.count(oldKey) ? L.qty[oldKey] : a.qb;
                    L.qty[newKey] = oldQtyVal;

                    bool stillUsed = false;
                    for (Node *tmp = L.head; tmp; tmp = tmp->next)
                    {
                        if (tmp != curr && tmp->a.name == a.OV.name && tmp->a.dosage == a.OV.dosage)
                        {
                            stillUsed = true;
                            break;
                        }
                    }
                    if (!stillUsed)
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
