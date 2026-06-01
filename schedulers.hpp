#pragma once

#include <algorithm>
#include <cassert>
#include <deque>
#include <memory>
#include <queue>
#include <string>
#include "scheduler.hpp"

// All scheduling policies live in this file, followed by the registry at the
// bottom that maps a command-line name to a policy.

// Fifo — first-in, first-out (run-to-completion) scheduling policy.
// See scheduler.hpp for the full contract:
//   init(tasks)          receive the whole workload up front; do any pre-work.
//   pick(returned)       re-admit `returned` if non-null, then return the
//                        Task* to run this tick, or nullptr (idle).
class Fifo : public Scheduler {
    std::deque<Task*> taskQueue;
public:
    void init(const std::vector<Task*>& tasks) override {
        for(Task* task : tasks) {
            taskQueue.push_back(task);
        }
    }

    Task* pick(Task* incoming) override {
        if(incoming != nullptr){
            return incoming;
        }
        if(taskQueue.empty()){
            return nullptr;
        }
        Task* nextTask = taskQueue.front();
        taskQueue.pop_front();
        return nextTask;
    }

    const char* name() const override { return "FIFO"; }
};

// class SJF : public Scheduler {
//     struct minRemaining {
//         bool operator()(Task* a, Task* b) const {
//             return a->remaining > b->remaining;
//         }
//     };
//     std::priority_queue<Task*, std::vector<Task*>, minRemaining> taskQueue;
// public:
//     void init(const std::vector<Task*>& tasks) override {
//         for(Task* task : tasks) {
//             taskQueue.push(task);
//         }
//     }
//     Task* pick(Task* incoming) override {
//         if(incoming != nullptr){
//             taskQueue.push(incoming);
//         }
//         if(taskQueue.empty()){
//             return nullptr;
//         }
//         Task* nextTask = taskQueue.top();
//         taskQueue.pop();
//         return nextTask;
//     }
//     const char* name() const override { return "SJF"; }
// };

class RoundRobin : public Scheduler {
    std::deque<Task*> taskQueue;
public:
    void init(const std::vector<Task*>& tasks) override {
        for(Task* task : tasks) {
            taskQueue.push_back(task);
        }
    }
    Task* pick(Task* incoming) override {
        if(incoming != nullptr){
            taskQueue.push_back(incoming);
        }
        if(taskQueue.empty()){
            return nullptr;
        }
        Task* nextTask = taskQueue.front();
        taskQueue.pop_front();
        return nextTask;
    }
    const char* name() const override { return "Round Robin"; }
};

class MLFQ : public Scheduler {
    int cur_quanta = 0;
    int cur_level = 0;
    struct Level{
        std::deque<Task*> taskQueue;
        int quanta;
        Level(int quanta) : quanta(quanta) {}
        void addTask(Task* task){
            taskQueue.push_back(task);
        }
        Task* getNextTask(){
            if(taskQueue.empty()){
                return nullptr;
            }
            Task* nextTask = taskQueue.front();
            taskQueue.pop_front();
            return nextTask;
        }
        bool hasNextTask() const {
            return !taskQueue.empty();
        }
    };
    Level levels[4] = {Level(2), Level(4), Level(8), Level(16)};
public:
    void init(const std::vector<Task*>& tasks) override {
        for(Task* task : tasks) {
            levels[0].addTask(task);
        }
    }
    Task* getNextTask(int level, Task* incoming = nullptr){
        if(incoming != nullptr){
            levels[cur_level].addTask(incoming);
        }
        Task* nextTask = levels[level].getNextTask();
        assert(nextTask != nullptr);
        cur_level = level;
        cur_quanta = 0;
        return nextTask;
    }
    Task* pick(Task* incoming) override {
        using namespace std;
        cur_quanta++;
        int level = 0;

        //if there is higher prio task, prempt
        while(level < cur_level){
            if(levels[level].hasNextTask()){
                return getNextTask(level, incoming);
            }
            level++;
        }

        // if incoming is existing task, need to deal with that, wheter returning or adding back to queue
        if(incoming != nullptr){
            if(cur_quanta < levels[cur_level].quanta){
                return incoming;
            }
            else{
                levels[min(3, cur_level+1)].addTask(incoming);
            }
        }

        // get next task. could be from current level, could be from lower prio level. the point
        // being that incoming has already been properly dealt with
        while(level < 4){
            if(levels[level].hasNextTask()){
                return getNextTask(level);
            }
            level++;
        }

        //! doesn't do cleanup
        return nullptr;
    }
    const char* name() const override { return "MLFQ"; }
};

// ---------------------------------------------------------------------------
// Registry. Maps a scheduler name (as given on the command line) to a fresh
// instance, or nullptr if unrecognized. Add a policy class above, then add one
// line here.
inline std::unique_ptr<Scheduler> makeScheduler(const std::string& name) {
    if (name == "Fifo") return std::make_unique<Fifo>();
    // if (name == "SJF") return std::make_unique<SJF>();
    if (name == "RoundRobin") return std::make_unique<RoundRobin>();
    if (name == "MLFQ") return std::make_unique<MLFQ>();
    return nullptr;
}
