#include "../hppfolder/FileHandler.hpp"
#include "../hppfolder/DS.hpp"
#include <filesystem>
#include <sstream>

static const string DATA_DIR = "data";
using namespace std;

static string sanitize(const string &s)
{
    string out = s;
    for (char &c : out)
        if (c == '|' || c == '\n' || c == '\r')
            c = ' ';
    return out;
}

void loadFromFile(LinkedList &L, const string &username)
{
    string filename = DATA_DIR + "/" + username + "_data.txt";
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "[Info] No medicine file for " << username
             << ". Starting fresh.\n";
        return;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        Med m;
        string qtyStr, timeStr, expStr, daysStr;

        getline(ss, m.name, '|');
        getline(ss, m.dosage, '|');
        getline(ss, qtyStr, '|');
        getline(ss, timeStr, '|');
        getline(ss, expStr, '|');
        getline(ss, daysStr, '|');

        int qty = 0;
        stringstream(qtyStr) >> qty;
        stringstream(timeStr) >> m.t.h >> m.t.m;
        stringstream(expStr) >> m.exp.d >> m.exp.m >> m.exp.y;

        m.dy.clear();
        string num;
        stringstream ds(daysStr);
        while (getline(ds, num, ','))
        {
            if (!num.empty())
                m.dy.push_back(stoi(num));
        }

        Node *node = new Node{m, nullptr};

        if (!L.head || m.t < L.head->a.t)
        {
            node->next = L.head;
            L.head = node;
        }
        else
        {
            Node *r = L.head;
            while (r->next && !(m.t < r->next->a.t))
                r = r->next;
            node->next = r->next;
            r->next = node;
        }

        L.hash[{m.name, m.dosage}].push_back(node);
        L.qty[{m.name, m.dosage}] = qty;
    }

    file.close();
    cout << "[Info] Medicines loaded successfully.\n";
}

void commitToFile(const LinkedList &L, const string &username)
{
    string mainFile = DATA_DIR + "/" + username + "_data.txt";
    string tempFile = DATA_DIR + "/" + username + "_temp.txt";
    ofstream out(tempFile, ios::trunc);
    if (!out.is_open())
    {
        cerr << "[Error] Unable to open temporary file for commit.\n";
        return;
    }

    Node *r = L.head;
    while (r)
    {
        const Med &m = r->a;
        int q = 0;
        auto it = L.qty.find({m.name, m.dosage});
        if (it != L.qty.end())
            q = it->second;

        out << sanitize(m.name) << "|"
            << sanitize(m.dosage) << "|"
            << q << "|"
            << m.t.h << " " << m.t.m << "|"
            << m.exp.d << " " << m.exp.m << " " << m.exp.y << "|";

        for (size_t i = 0; i < m.dy.size(); i++)
        {
            out << m.dy[i];
            if (i != m.dy.size() - 1)
                out << ",";
        }
        out << "\n";
        r = r->next;
    }

    out.close();

    try
    {
        if (std::filesystem::exists(mainFile))
            std::filesystem::remove(mainFile);
        std::filesystem::rename(tempFile, mainFile);
        cout << "All changes committed successfully.\n";
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        cerr << "[Error] Commit failed, keeping old file: " << e.what() << endl;
        if (std::filesystem::exists(tempFile))
            std::filesystem::remove(tempFile);
    }
}
