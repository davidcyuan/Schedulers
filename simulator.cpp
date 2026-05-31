#include "simulator.hpp"

#include <cstdio>
#include <unordered_map>

namespace {

// Per-task bookkeeping the simulator tracks (the task itself owns only its
// remaining/total time).
struct Timing {
    int start = -1;       // first tick the task ran
    int finish = -1;      // tick at which the task completed
    int service = 0;      // ticks the task actually ran
    double ideal = 0.0;   // fair-share work owed so far (+= 1/active each tick alive)
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
    double maxLag = 0.0;  // worst (ideal - service) seen: how starved a task got
    while (completed < n) {
        held = sched.pick(held);
        if (held == nullptr) {
            ++now;  // CPU idle this tick
            continue;
        }

        // Fairness: an ideal CPU would split this tick equally among all tasks
        // that still have work. Credit each its 1/active share, the running one
        // its actual unit, and record the worst resulting lag (= starvation).
        int active = n - completed;  // tasks with work left at tick start
        double share = 1.0 / active;
        for (Task* t : ptrs) {
            if (t->remaining == 0) continue;  // already finished, no longer owed
            Timing& ti = timing[t];
            ti.ideal += share;
            if (t == held) ++ti.service;
            double lag = ti.ideal - ti.service;
            if (lag > maxLag) maxLag = lag;
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
        double slowdown =
            t->burst > 0 ? static_cast<double>(turnaround) / t->burst : 1.0;
        result.tasks.push_back(
            {t->id, t->burst, tm.start, tm.finish, turnaround, slowdown});
        if (tm.finish > makespan) makespan = tm.finish;
        sumTurnaround += turnaround;
    }
    result.total_turnaround = makespan;  // time to process all tasks
    result.average_turnaround =
        n > 0 ? static_cast<double>(sumTurnaround) / n : 0.0;
    result.max_lag = maxLag;
    return result;
}

void printResult(const RunResult& r, const char* schedName) {
    std::printf("=== %s ===\n", schedName);
    std::printf("%3s %8s %6s %6s %10s %8s\n", "id", "required", "start",
                "finish", "turnaround", "slowdown");
    for (const auto& t : r.tasks) {
        std::printf("%3d %8d %6d %6d %10d %8.2f\n", t.id, t.required, t.start,
                    t.finish, t.turnaround, t.slowdown);
    }
    std::printf("total turnaround (makespan): %g\n", r.total_turnaround);
    std::printf("average turnaround:          %.2f\n", r.average_turnaround);
    std::printf("max lag (unfairness):        %.2f\n", r.max_lag);
}
