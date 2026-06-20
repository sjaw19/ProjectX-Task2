#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

struct Task {
    string id;
    int burst;
    vector<string> mem;
};

deque<string> L1, L2, L3;
int ramAccesses = 0;

bool contains(deque<string>& q, const string& x) {
    return find(q.begin(), q.end(), x) != q.end();
}

void insertL3(const string& x) {
    if (contains(L3, x)) return;

    if (L3.size() >= 512)
        L3.pop_front();

    L3.push_back(x);
}

void insertL2(const string& x) {
    if (contains(L2, x)) return;

    if (L2.size() >= 128) {
        string evicted = L2.front();
        L2.pop_front();
        insertL3(evicted);
    }

    L2.push_back(x);
}

void insertL1(const string& x) {
    if (contains(L1, x)) return;

    if (L1.size() >= 32) {
        string evicted = L1.front();
        L1.pop_front();
        insertL2(evicted);
    }

    L1.push_back(x);
}

void accessMemory(const string& block) {

    if (contains(L1, block)) {
        cout << "L1 -> HIT (4 cycles)\n";
        return;
    }

    if (contains(L2, block)) {
        cout << "L1 -> MISS\n";
        cout << "L2 -> HIT (12 cycles)\n";

        L2.erase(find(L2.begin(), L2.end(), block));
        insertL1(block);
        return;
    }

    if (contains(L3, block)) {
        cout << "L1 -> MISS\n";
        cout << "L2 -> MISS\n";
        cout << "L3 -> HIT (40 cycles)\n";

        L3.erase(find(L3.begin(), L3.end(), block));
        insertL1(block);
        return;
    }

    cout << "L1 -> MISS\n";
    cout << "L2 -> MISS\n";
    cout << "L3 -> MISS\n";
    cout << "Fetching from RAM\n";

    ramAccesses++;
    insertL1(block);
}

void printCache(const string& name, deque<string>& q) {
    cout << name << ": [";

    for (size_t i = 0; i < q.size(); i++) {
        cout << q[i];
        if (i + 1 < q.size())
            cout << ", ";
    }

    cout << "]\n";
}

vector<Task> loadTasks(const string& filename) {

    ifstream fin(filename);

    if (!fin) {
        cout << "Cannot open input file\n";
        exit(1);
    }

    vector<Task> tasks;
    string line;

    while (getline(fin, line)) {

        if (line.empty()) continue;

        stringstream ss(line);

        Task t;
        string temp;

        ss >> temp >> t.id >> temp >> t.burst >> temp;

        string block;
        while (ss >> block)
            t.mem.push_back(block);

        tasks.push_back(t);
    }

    return tasks;
}

int main() {

    vector<Task> tasks = loadTasks("input_task2.txt");

    sort(tasks.begin(), tasks.end(),
         [](const Task& a, const Task& b) {
             return a.burst < b.burst;
         });

    int cycle = 1;

    cout << "===== SJF CPU SCHEDULER =====\n";

    for (auto& task : tasks) {

        for (int i = 0; i < task.burst; i++) {

            string block = task.mem[i % task.mem.size()];

            cout << "\n----------------------------------\n";
            cout << "Cycle " << cycle << '\n';
            cout << "Running: " << task.id << '\n';
            cout << "Requesting: " << block << '\n';

            accessMemory(block);

            printCache("L1", L1);
            printCache("L2", L2);
            printCache("L3", L3);

            cycle++;
        }
    }

    
    cout << "\n=== FINAL RESULTS ===\n";

    cout << "Total Cycles: " << cycle - 1 << '\n';
    cout << "Tasks Completed: " << tasks.size() << '\n';
    cout << "Scheduler: Shortest Job First (SJF)\n";
    cout << "RAM Accesses: " << ramAccesses << '\n';

    return 0;
}