#pragma once

#include <deque>
#include "scheduler.hpp"

// Scheduler1 — your first scheduling policy. Fill in the hooks.
// See scheduler.hpp for the full contract:
//   init(tasks)          receive the whole workload up front; do any pre-work.
//   pick(returned, now)  re-admit `returned` if non-null, then return the
//                        Task* to run this tick, or nullptr (idle).
class Scheduler1 : public Scheduler {
    //gonna start with FIFO
    std::deque<Task*> taskQueue;
public:
    void init(const std::vector<Task*>& tasks) override {
        for(Task* task : tasks) {
            taskQueue.push_back(task);
        }
    }

    Task* pick(Task* incoming, int now) override {
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

    const char* name() const override { return "Scheduler1"; }
};
