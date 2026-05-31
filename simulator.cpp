#include "simulator.hpp"

#include <cstdio>
#include <unordered_map>

namespace {

// Per-task bookkeeping the simulator tracks (the task itself owns only its
// remaining/total time).
struct Timing {
    int start = -1;   // first tick the task ran
    int finish = -1;  // tick at which the task completed
};

}  // namespace

RunResult simulate(std::vector<Task>& workload, Scheduler& sched,
                   bool detailed) {
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
            if (detailed) std::printf("t=%3d: idle\n", now);
            ++now;  // CPU idle this tick
            continue;
        }

        if (detailed) std::printf("t=%3d: task %d\n", now, held->id);

        Timing& tm = timing[held];
        if (tm.start < 0) tm.start = now;
        --held->remaining;
        ++now;

        if (held->remaining == 0) {
            tm.finish = now;  // finalized the instant it completes
            ++completed;
            held = nullptr;
        }
    }

    RunResult result;
    result.tasks.reserve(n);
    long totalPenalty = 0;
    for (Task* t : ptrs) {
        const Timing& tm = timing[t];
        // One coefficient is charged for every tick the task existed unfinished.
        // All tasks arrive at t=0, so that span is exactly its finish time.
        int penalty = t->penalty_coefficient * tm.finish;
        result.tasks.push_back({t->id, t->burst, tm.start, tm.finish, penalty});
        totalPenalty += penalty;
    }
    result.total_penalty = totalPenalty;
    return result;
}

void printResult(const RunResult& r, const char* schedName) {
    std::printf("=== %s ===\n", schedName);
    std::printf("%3s %8s %6s %6s %8s\n", "id", "required", "start", "finish",
                "penalty");
    for (const auto& t : r.tasks) {
        std::printf("%3d %8d %6d %6d %8d\n", t.id, t.required, t.start,
                    t.finish, t.penalty);
    }
    std::printf("total penalty: %ld\n", r.total_penalty);
}
