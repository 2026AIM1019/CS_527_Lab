# CS527 OpenMP Lab — C++ Solutions

This project contains C++ implementations of all five tasks from the CS527 introduction to OpenMP lab. A single menu-driven program lets you choose which task to run. It does **not** automatically execute all tasks.

## Requirements
- `g++` with OpenMP support
- GNU Make
- Enough RAM for the selected task and input size

## Build and run
From this directory, run:

```bash
make
./build/cs527_openmp
```

Or build and launch the menu with:

```bash
make run
```

The menu repeats after each task. Enter `1` through `5` to run only that task, or `0` to exit. Invalid choices return to the menu. `make clean` removes the compiled executable.

## Thread counts and timing
The task programs use `omp_set_num_threads()` to test 1, 2, 4, and 8 threads. Execution time is measured with `omp_get_wtime()`. Most task timings use a warm-up followed by three measured runs and report the median; Task 4 batches the tiny 8 x 8 operation to avoid timer-resolution noise. Results depend on the CPU, compiler, memory bandwidth, and background load; no benchmark numbers are pre-filled in the reports. Console tables use fixed-width columns (not tab characters) so headings and values stay aligned. Run the program on your machine and record the actual results in each task's `REPORT.md`.

## Tasks and reports
1. `task1_vector_add/` — serial and parallel vector addition for N = 10^5, 10^6, 10^7, 10^8. See `task1_vector_add/REPORT.md`.
2. `task2_array_sum/` — serial sum, intentionally unsafe naive parallel sum for observing a data race, and correct reduction sum for N = 10^7 and 10^8. See `task2_array_sum/REPORT.md`.
3. `task3_max_min/` — maximum/minimum values and indices using critical-section and per-thread approaches for N = 10^7 and 10^8. See `task3_max_min/REPORT.md`.
4. `task4_matrix_add/` — serial, outer-loop parallel, and `collapse(2)` matrix addition for 8 x 8 and 4096 x 4096. See `task4_matrix_add/REPORT.md`.
5. `task5_matrix_multiply/` — serial, outer-loop parallel, and `collapse(2)` matrix multiplication for N = 256, 512, 1024. See `task5_matrix_multiply/REPORT.md`.

## Implementation notes
- Each task's code is kept in its own directory. The task source defines a `runTaskN()` function; the top-level `main.cpp` contains the menu and dispatches the selected function through a `while` loop and `switch`.
- Matrices use flattened row-major `std::vector` storage.
- Task 2's naive sum intentionally contains a data race because the lab asks you to observe it. Its result is undefined and must not be treated as correct.
- Floating-point matrix multiplication is checked with a tolerance.
- Matrix multiplication at N = 1024 may take some time with the straightforward triple-loop algorithm.
