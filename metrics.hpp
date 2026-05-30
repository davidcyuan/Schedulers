#pragma once

#include <vector>

// Per-task results computed after a run. With all tasks present at t=0,
// turnaround == finish and response == start.
struct TaskMetrics {
    int id;
    int burst;
    int start;       // first tick the task ran
    int finish;      // tick at which the task completed
    int turnaround;  // finish (time in system)
    int waiting;     // turnaround - burst
    int response;    // start (time until first run)
};

struct RunResult {
    std::vector<TaskMetrics> tasks;  // in input order
    double avgTurnaround = 0.0;
    double avgWaiting = 0.0;
    double avgResponse = 0.0;
};
