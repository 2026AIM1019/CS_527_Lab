# CS527 – Computer Systems: Lab 8 (OpenMP — Arrays and Matrices)

This project implements the five requested OpenMP tasks in C++17. Each task includes a serial reference, a parallel implementation, correctness checks, and timing tables for 1, 2, 4, and 8 threads. Timings use `omp_get_wtime()`, one warm-up run, and the median of three measured runs.

## Build and run

```bash
make
make run
```

Compiler flags include `-fopenmp -Wall -Wextra -Wpedantic`. The program presents a menu; enter 1–5 to run a task or 0 to exit. You can also run the individual task source files only as part of the complete build because each defines its task entry point and shares `common.h`.

## Tasks and synchronization

1. **Array search** (`task1_array_search/main.cpp`): searches for a value and reports its first zero-based index. An OpenMP `reduction(min:first_index)` safely combines the earliest match; `n` is the internal not-found sentinel and is displayed as `-1` for an absent result.
2. **Array sorting** (`task2_array_sort/main.cpp`): recursive task-parallel merge sort. Each task sorts a disjoint range. `taskwait` ensures both halves are complete before the parent merges them. Concurrent tasks write disjoint portions of the temporary array, so no lock is needed. The result is compared element-by-element with serial `sort`.
3. **Histogram** (`task3_histogram/main.cpp`): each OpenMP thread updates its own 10-bin histogram, avoiding races. The per-thread histograms are combined after the parallel region.
4. **Matrix transpose** (`task4_matrix_transpose/main.cpp`): `collapse(2)` parallelizes row/column pairs. Every output element has exactly one writer.
5. **2D convolution** (`task5_convolution/main.cpp`): applies a 3×3 all-ones averaging kernel divided by 9. It uses valid positions without padding, producing an `(rows-2) × (columns-2)` output. Each output pixel is computed independently. The specified 5×5 example is checked against the expected 3×3 result.

## Correctness and timing notes

- `PASS` means the parallel result agrees with the serial reference (and, for the example, with the stated expected output).
- Speedup = serial median time / parallel median time.
- Parallel efficiency = speedup / thread count. Efficiency above 1 can occur for very short workloads due to measurement noise or implementation differences; it is not a physical efficiency above 100%.
- Performance depends on the machine, compiler, runtime, and current system load. The program prints fresh measurements when run; do not treat sample timings from another machine as guaranteed results.
- Sorting input sizes: 100,000; 500,000; 1,000,000. Search: 1,000,000; 5,000,000; 10,000,000. Histogram: 1,000,000; 10,000,000; 50,000,000. Transpose: 256²; 1024²; 2048². Convolution: 128²; 512²; 1024².
