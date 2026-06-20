#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

struct Task {
    string id;
    int burst;
    vector<string> memBlocks;
};

class CacheLevel {
public:
    int capacity;
    deque<string> blocks;

    CacheLevel(int cap) : capacity(cap) {}

    bool contains(const string& block) {
        for (const auto& b : blocks)
            if (b == block)
                return true;
        return false;
    }

    void insert(const string& block) {
        if (contains(block))
            return;

        if ((int)blocks.size() >= capacity)
            blocks.pop_front();

        blocks.push_back(block);
    }

    void remove(const string& block) {
        auto it = find(blocks.begin(), blocks.end(), block);
        if (it != blocks.end())
            blocks.erase(it);
    }

    void print(const string& name) {
        cout << name << ": [";

        for (size_t i = 0; i < blocks.size(); i++) {
            cout << blocks[i];
            if (i + 1 < blocks.size())
                cout << ", ";
        }

        cout << "]";
    }
};

class CacheHierarchy {
public:
    CacheLevel L1;
    CacheLevel L2;
    CacheLevel L3;

    int ramAccesses = 0;

    CacheHierarchy() :
        L1(32),
        L2(128),
        L3(512) {}

    int accessBlock(const string& block) {

        if (L1.contains(block)) {
            cout << "L1 -> HIT (4 cycles)\n";
            return 4;
        }

        if (L2.contains(block)) {
            cout << "L1 -> MISS\n";
            cout << "L2 -> HIT (12 cycles)\n";

            L2.remove(block);
            L1.insert(block);

            return 12;
        }

        if (L3.contains(block)) {
            cout << "L1 -> MISS\n";
            cout << "L2 -> MISS\n";
            cout << "L3 -> HIT (40 cycles)\n";

            L3.remove(block);
            L1.insert(block);

            return 40;
        }

        cout << "L1 -> MISS\n";
        cout << "L2 -> MISS\n";
        cout << "L3 -> MISS\n";
        cout << "Fetching from RAM (200 cycles)\n";

        ramAccesses++;

        L1.insert(block);

        return 200;
    }

    void printCaches() {
        L1.print("L1");
        cout << endl;

        L2.print("L2");
        cout << endl;

        L3.print("L3");
        cout << endl;
    }
};

vector<Task> loadTasks(const string& filename) {

    ifstream fin(filename);

    vector<Task> tasks;

    string line;

    while (getline(fin, line)) {

        stringstream ss(line);

        string temp;

        Task t;

        ss >> temp;
        ss >> t.id;

        ss >> temp;
        ss >> t.burst;

        ss >> temp;

        string block;

        while (ss >> block)
            t.memBlocks.push_back(block);

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

    CacheHierarchy cache;

    int cycle = 1;
    int totalCycles = 0;

    cout << "===== SJF SCHEDULER =====\n\n";

    for (auto& task : tasks) {

        for (int i = 0; i < task.burst; i++) {

            string block =
                task.memBlocks[i % task.memBlocks.size()];

            cout << "---------------------------------\n";

            cout << "Cycle " << cycle << endl;

            cout << "Running: "
                 << task.id
                 << endl;

            cout << "Requesting: "
                 << block
                 << endl;

            int latency =
                cache.accessBlock(block);

            cache.printCaches();

            cout << "Latency Paid: "
                 << latency
                 << " cycles\n";

            cout << endl;

            cycle++;
            totalCycles++;
        }
    }

    cout << "\n============================\n";
    cout << "=== FINAL RESULTS ==========\n";
    cout << "============================\n";

    cout << "Scheduler: SJF\n";

    cout << "Tasks Completed: "
         << tasks.size()
         << endl;

    cout << "CPU Cycles: "
         << totalCycles
         << endl;

    cout << "RAM Accesses: "
         << cache.ramAccesses
         << endl;

    return 0;
}