# Task 1 – Array Search

The program searches for a target using a serial `find` reference and an OpenMP parallel loop. A `min` reduction combines each thread's smallest matching index, so the final result is the first (smallest) matching index. If the value is absent, the result is the array length internally and `-1` when displayed. The benchmark checks an existing target at the final array position and separately checks an absent target. The output table includes thread count, median parallel time, speedup, efficiency, and correctness.
