#include "simulator.hpp"

#include <cstdio>
#include <unordered_map>

namespace {

// Completion timestamps the simulator tracks per task (the task itself owns
// only its remaining/total time).
struct Timing {
    int start = -1;   // first tick the task ran
    int finish = -1;  // tick at which the task completed
};

}  // namespace

RunResult simulate(std::vector<Task>& workload, Scheduler& sched) {
    const int n = static_cast<int>(workload.size());

    // Build the Task* list for the policy and reset per-task state for a run.
    std::vector<Task*> ptrs;
    ptrs.reserve(n);
    std::unordered_map<Task*, Timing> timing;
    for (Task& t : workload) {
        t.remaining = t.burst;
        ptrs.push_back(&t);
        timing[&t] = Timing{};
    }

    sched.init(ptrs);

    // The CPU holds at most one task. Each tick it hands the held task back to
    // pick(): the still-incomplete task, or nullptr if the previous one just
    // finished (and was finalized) or the CPU was idle. pick() returns the next
    // task to run, or nullptr to idle. A task hitting remaining == 0 is
    // finalized in place and never handed back.
    Task* held = nullptr;
    int now = 0;
    int completed = 0;
    while (completed < n) {
        held = sched.pick(held, now);
        if (held == nullptr) {
            ++now;  // CPU idle this tick
            continue;
        }

        Timing& tm = timing[held];
        if (tm.start < 0) tm.start = now;
        --held->remaining;
        ++now;

        if (held->remaining == 0) {
            timing[held].finish = now;  // finalized the instant it completes
            ++completed;
            held = nullptr;
        }
    }

    RunResult result;
    result.tasks.reserve(n);
    long sumT = 0, sumW = 0, sumR = 0;
    for (Task* t : ptrs) {
        const Timing& tm = timing[t];
        int turnaround = tm.finish;
        int waiting = turnaround - t->burst;
        int response = tm.start;
        result.tasks.push_back(
            {t->id, t->burst, tm.start, tm.finish, turnaround, waiting, response});
        sumT += turnaround;
        sumW += waiting;
        sumR += response;
    }
    if (n > 0) {
        result.avgTurnaround = static_cast<double>(sumT) / n;
        result.avgWaiting = static_cast<double>(sumW) / n;
        result.avgResponse = static_cast<double>(sumR) / n;
    }
    return result;
}

void printResult(const RunResult& r, const char* schedName) {
    std::printf("=== %s ===\n", schedName);
    std::printf("%3s %6s %6s %6s %10s %8s %9s\n", "id", "burst", "start",
                "finish", "turnaround", "waiting", "response");
    for (const auto& t : r.tasks) {
        std::printf("%3d %6d %6d %6d %10d %8d %9d\n", t.id, t.burst, t.start,
                    t.finish, t.turnaround, t.waiting, t.response);
    }
    std::printf("avg %37.2f %8.2f %9.2f\n", r.avgTurnaround, r.avgWaiting,
                r.avgResponse);
}
