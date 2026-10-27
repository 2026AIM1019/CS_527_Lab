# CS527 — Computer Systems, Lab 9: OpenMP

## Build and run

Requires g++ with OpenMP support. From the project root:

```sh
make
make run
```

The menu repeats until you enter `0`. To clean, use `make clean`.

Each task has its own source file and `REPORT.md`. The root `main.cpp` handles the menu. The `common.h` helper prints aligned timing tables.

## Experiments

- Task 1: serial merge sort versus OpenMP task merge sort, N = 100,000 / 1,000,000 / 10,000,000; task cutoff = 4096.
- Task 2: serial uneven computational workload versus static, dynamic (chunk=100), guided; N = 100,000 / 1,000,000 / 10,000,000.
- Task 3: serial even/odd count versus OpenMP multi-variable reduction and block-aggregated atomic updates; N = 100,000 / 1,000,000 / 10,000,000 / 100,000,000.

Parallel thread counts: 1, 2, 4, 8. Every parallel result is compared against a serial baseline and marked PASS/FAIL. Times are measured with `omp_get_wtime()` and speedup = serial_time / parallel_time; efficiency = speedup / thread_count.

## Notes

Task 3 deliberately uses *block-aggregated* atomic updates (one atomic update per count per 4096 elements) instead of atomics for every element. This still demonstrates `#pragma omp atomic` correctness but is **not** a per-element atomic benchmark; the distinction matters for interpreting the performance comparison. The program allocates an array of 100 million integers (~400 MB) in Task 3; ensure adequate memory. Task 2 uses a deterministic uneven workload of 12–110 floating-point iterations per element, organized in 10,000-element bands. Results depend on your CPU and load. Benchmark times are single measurements and should be repeated for stable research-quality conclusions.

The captured test output is in `TEST_RESULTS.txt`.
