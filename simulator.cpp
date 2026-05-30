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
        held = sched.pick(held);
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
    int makespan = 0;
    long sumTurnaround = 0;
    for (Task* t : ptrs) {
        const Timing& tm = timing[t];
        int turnaround = tm.finish;  // all tasks present at t=0, so == finish
        result.tasks.push_back({t->id, t->burst, tm.start, tm.finish, turnaround});
        if (tm.finish > makespan) makespan = tm.finish;
        sumTurnaround += turnaround;
    }
    result.total_turnaround = makespan;  // time to process all tasks
    result.average_turnaround =
        n > 0 ? static_cast<double>(sumTurnaround) / n : 0.0;
    return result;
}

void printResult(const RunResult& r, const char* schedName) {
    std::printf("=== %s ===\n", schedName);
    std::printf("%3s %8s %6s %6s %10s\n", "id", "required", "start", "finish",
                "turnaround");
    for (const auto& t : r.tasks) {
        std::printf("%3d %8d %6d %6d %10d\n", t.id, t.required, t.start,
                    t.finish, t.turnaround);
    }
    std::printf("total turnaround (makespan): %g\n", r.total_turnaround);
    std::printf("average turnaround:          %.2f\n", r.average_turnaround);
}
