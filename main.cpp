#include <cstdio>
#include <string>
#include <vector>

#include "task.hpp"
#include "simulator.hpp"
#include "schedulers.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <scheduler-name> [detailed]\n", argv[0]);
        return 1;
    }

    std::string name = argv[1];
    std::unique_ptr<Scheduler> sched = makeScheduler(name);
    if (!sched) {
        std::fprintf(stderr, "unknown scheduler: '%s'\n", name.c_str());
        return 1;
    }

    // Optional "detailed" flag: trace the running task every tick.
    bool detailed = false;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "detailed" || arg == "--detailed") detailed = true;
    }

    // A small sample workload. All tasks are present at t=0. Penalty
    // coefficients are (loosely) inversely correlated with burst: shorter
    // tasks tend to carry a higher per-tick penalty.
    std::vector<Task> workload = {
        {.id = 1, .burst = 20, .penalty_coefficient = 1},
        {.id = 2, .burst = 10, .penalty_coefficient = 3},
        {.id = 3, .burst = 5,  .penalty_coefficient = 6},
        {.id = 4, .burst = 2,  .penalty_coefficient = 10},
        // {.id = 1, .burst = 20, .penalty_coefficient = 1},
        // {.id = 2, .burst = 20, .penalty_coefficient = 1},
        // {.id = 3, .burst = 20,  .penalty_coefficient = 1},
        // {.id = 4, .burst = 20,  .penalty_coefficient = 1},
    };

    RunResult result = simulate(workload, *sched, detailed);
    printResult(result, sched->name());

    return 0;
}
