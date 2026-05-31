#include <cstdio>
#include <string>
#include <vector>

#include "task.hpp"
#include "simulator.hpp"
#include "schedulers.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <scheduler-name>\n", argv[0]);
        std::fprintf(stderr, "available: fifo\n");
        return 1;
    }

    std::string name = argv[1];
    std::unique_ptr<Scheduler> sched = makeScheduler(name);
    if (!sched) {
        std::fprintf(stderr, "unknown scheduler: '%s'\n", name.c_str());
        return 1;
    }

    // A small sample workload. id, burst. All tasks are present at t=0.
    std::vector<Task> workload = {
        {4, 10},
        {1, 20},
        {2, 10},
        {3, 10},
    };

    RunResult result = simulate(workload, *sched);
    printResult(result, sched->name());

    return 0;
}
