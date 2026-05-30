#pragma once

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

class SJF : public Scheduler {
    struct minRemaining {
        bool operator()(Task* a, Task* b) const {
            return a->remaining > b->remaining;
        }
    };
    std::priority_queue<Task*, std::vector<Task*>, minRemaining> taskQueue;
public:
    void init(const std::vector<Task*>& tasks) override {
        for(Task* task : tasks) {
            taskQueue.push(task);
        }
    }
    Task* pick(Task* incoming) override {
        if(incoming != nullptr){
            taskQueue.push(incoming);
        }
        if(taskQueue.empty()){
            return nullptr;
        }
        Task* nextTask = taskQueue.top();
        taskQueue.pop();
        return nextTask;
    }
    const char* name() const override { return "SJF"; }
};

// ---------------------------------------------------------------------------
// Registry. Maps a scheduler name (as given on the command line) to a fresh
// instance, or nullptr if unrecognized. Add a policy class above, then add one
// line here.
inline std::unique_ptr<Scheduler> makeScheduler(const std::string& name) {
    if (name == "Fifo") return std::make_unique<Fifo>();
    if (name == "SJF") return std::make_unique<SJF>();
    return nullptr;
}
