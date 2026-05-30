#pragma once

#include <vector>

// Per-task results computed after a run. With all tasks present at t=0,
// turnaround == finish and response == start.
struct TaskMetrics {
    int id;
    int required;    // total work required
    int start;       // first tick the task ran
    int finish;      // tick at which the task completed
    int turnaround;  // finish (time in system)
};

struct RunResult {
    std::vector<TaskMetrics> tasks;  // in input order
    double total_turnaround;        // the time it takes to process all tasks
    double average_turnaround;      // average turnaround time per task
};
