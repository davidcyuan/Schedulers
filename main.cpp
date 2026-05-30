#include <vector>

#include "task.hpp"
#include "scheduler_1.hpp"
#include "simulator.hpp"

int main() {
    // A small sample workload. id, burst. All tasks are present at t=0.
    std::vector<Task> workload = {
        {1, 5},
        {2, 3},
        {3, 6},
        {4, 2},
    };

    Scheduler1 sched;
    RunResult result = simulate(workload, sched);
    printResult(result, sched.name());

    return 0;
}
