#pragma once

#include <vector>

// Per-task results computed after a run. With all tasks present at t=0,
// turnaround == finish and response == start.
struct TaskMetrics {
    int id;
    int required;       // total work required
    int start;          // first tick the task ran
    int finish;         // tick at which the task completed
    int turnaround;     // finish (time in system)
    double slowdown;    // turnaround / required (>= 1; 1.0 == ran as if alone)
};

struct RunResult {
    std::vector<TaskMetrics> tasks;  // in input order
    double total_turnaround;        // the time it takes to process all tasks
    double average_turnaround;      // average turnaround time per task
    // Fairness metric. The most any task fell behind an equal-share CPU,
    // measured in units of work (ticks): the worst starvation relative to
    // giving every live task 1/active of each tick. 0 == perfectly fair.
    // Not normalized, so its scale grows with workload size; normalize by
    // makespan or task count to compare across different workloads.
    double max_lag;
};
