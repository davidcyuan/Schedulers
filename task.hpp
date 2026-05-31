#pragma once

// A unit of work to schedule. CPU-only: a single CPU burst, run to
// completion (possibly across several time slices). All tasks are present
// from t=0 (no staggered arrival).
//
// The task tracks its own progress: `remaining` counts down as it runs,
// while `burst` keeps the original total. The simulator resets `remaining`
// to `burst` at the start of a run.
struct Task {
    int id;
    int burst;          // total CPU time required
    int remaining = 0;  // CPU time left to run
    int penalty_coefficient;
};
