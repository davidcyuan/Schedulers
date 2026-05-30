#pragma once

#include <vector>
#include "task.hpp"

// Scheduling policy interface. The simulator models a single CPU that holds
// at most one task at a time, and drives a logical clock one tick at a time.
//
//   init(tasks)      receive the whole workload up front (pointers to the
//                    caller's Task objects); do any pre-work (sort, bucket,
//                    build the ready queue). Every task is runnable from t=0.
//
//   pick(returned)   Combined hand-back + selection, called once per tick:
//                    - `returned` is the task the CPU is handing back because
//                      it is still incomplete; re-admit it to your ready
//                      structure. It is nullptr when the CPU has nothing to
//                      return (the previous task just finished, the CPU was
//                      idle, or it's the first tick).
//                    - then return the task to run this tick, or nullptr to
//                      idle. The returned task is now "held" by the CPU.
//
// A finished task (remaining == 0) is never handed back: the simulator
// finalizes it and logs its metrics. The policy never sees a task once done.
//
// The policy owns its ready structure and any quantum bookkeeping. The
// simulator owns the clock and per-task completion timestamps. Each task
// owns its own remaining/total time.
class Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual void  init(const std::vector<Task*>& tasks) = 0;
    virtual Task* pick(Task* returned) = 0;

    virtual const char* name() const = 0;
};
