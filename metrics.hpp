#pragma once

#include <vector>

// Per-task results computed after a run. With all tasks present at t=0,
// turnaround == finish and response == start.
struct TaskMetrics {
    int id;
    int required;       // total work required
    int start;          // first tick the task ran
    int finish;         // tick at which the task completed
    int penalty;
};

struct RunResult {
    std::vector<TaskMetrics> tasks;  // in input order
    long total_penalty;              // sum of per-task penalty -- the objective
                                     // the scheduler is trying to minimize
};
