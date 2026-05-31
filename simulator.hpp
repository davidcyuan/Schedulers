#pragma once

#include <vector>
#include "task.hpp"
#include "scheduler.hpp"
#include "metrics.hpp"

// Runs a workload through a scheduling policy on a logical clock and returns
// per-task plus aggregate metrics. The workload is taken by mutable reference:
// the simulator resets each task's remaining time and passes Task pointers to
// the policy. The simulator owns the clock and completion timestamps; the
// policy decides who runs; each task tracks its own remaining/total time.
//
// If `detailed` is set, the simulator prints the running task id every tick.
RunResult simulate(std::vector<Task>& workload, Scheduler& sched,
                   bool detailed = false);

// Pretty-print a RunResult as a metrics table.
void printResult(const RunResult& r, const char* schedName);
