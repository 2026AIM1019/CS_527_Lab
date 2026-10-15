# Task 4 Report — Matrix Addition

## Objective
Compute `C[i][j] = A[i][j] + B[i][j]` and compare OpenMP parallelization of only the outer loop with `collapse(2)`, which distributes the combined two-dimensional iteration space.

## Implementation
- Matrices are stored in flattened one-dimensional `std::vector<double>` arrays in row-major order. Element `(i,j)` is at offset `i * N + j`.
- `addSerial()` is the reference implementation.
- `addOuter()` applies `#pragma omp parallel for` to the outer `i` loop.
- `addCollapse()` applies `#pragma omp parallel for collapse(2)` to both loops.
- Sizes: `8 x 8` and `4096 x 4096`; thread counts: 1, 2, 4, 8.
- Both parallel outputs are compared against the serial result using a small floating-point tolerance.

## Timing methodology
Each implementation is warmed up and timed three times; the median per-call time is reported. For the tiny `8 x 8` matrix, each sample batches 200 calls and divides by 200 to reduce timer-resolution noise. Correctness checking is outside the timed region. Even with batching, OpenMP overhead can dominate the small-matrix workload.

Here is the GitHub-ready Markdown for Task 4: Matrix Addition, with properly formatted tables and a summary.

Writing

# Results to Record

> **Note:** Timings are median per-call times from 3 samples after a warm-up. For `N = 8`, each sample batches 200 calls to reduce timer-resolution noise.

## Matrix Size: 8 × 8

**Serial Median Time:** `0.000000063 s`

### Parallel Performance

| Threads | Outer Time (s) | Outer Check | Collapse Time (s) | Collapse Check |
| --- | --- | --- | --- | --- |
| 1 | 0.000001065 | PASS | 0.000001094 | PASS |
| 2 | 0.000001898 | PASS | 0.000001985 | PASS |
| 4 | 0.000002038 | PASS | 0.000001931 | PASS |
| 8 | 0.000052697 | PASS | 0.000044232 | PASS |

## Matrix Size: 4096 × 4096

**Serial Median Time:** `0.033349587 s`

### Parallel Performance

| Threads | Outer Time (s) | Outer Check | Collapse Time (s) | Collapse Check |
| --- | --- | --- | --- | --- |
| 1 | 0.034808299 | PASS | 0.036978456 | PASS |
| 2 | 0.030799888 | PASS | 0.029422685 | PASS |
| 4 | 0.030898969 | PASS | 0.037818241 | PASS |
| 8 | 0.037551446 | PASS | 0.036532494 | PASS |

## Summary and Observations

- **Correctness:** All configurations passed the correctness checks for both the outer-loop and collapsed-loop implementations.
- **Small matrices (8 × 8):** Parallel execution is slower than serial execution because the matrix is too small to benefit from multithreading, and thread-management overhead dominates the computation.
- **Large matrices (4096 × 4096):** Parallel execution with 2 threads improves performance for both implementations.
- **Best outer-loop performance:** 2 threads, with a time of `0.030799888 s` for the 4096 × 4096 matrix.
- **Best collapsed-loop performance:** 2 threads, with a time of `0.029422685 s` for the 4096 × 4096 matrix.
- **Higher thread counts:** Using 4 or 8 threads does not consistently improve performance. Memory bandwidth, thread scheduling, and parallelization overhead may limit scalability.

## Conclusion

Matrix addition is a highly parallelizable operation because each output element can be computed independently. However, the performance benefit depends on matrix size and thread count. For small matrices, serial execution is more efficient, while larger matrices can benefit from parallel execution. In these benchmarks, 2 threads provide the best performance for both parallel implementations on the 4096 × 4096 matrix.

## Questions and answers

### 1. Does `collapse(2)` help for small N?
For `N = 8`, the outer loop has only eight iterations. With more threads than useful outer-loop iterations, some threads may receive no row to process. `collapse(2)` exposes 64 `(i,j)` iterations, giving the runtime more opportunities to distribute work. For such a small matrix, however, thread startup and scheduling overhead can outweigh the saved work, so a lower time is not guaranteed.

### 2. Does `collapse(2)` matter for large N?
For `N = 4096`, the outer loop already has 4096 independent rows, which is enough work to keep 1, 2, 4, or 8 threads occupied. `collapse(2)` may therefore offer little improvement and can add scheduling overhead. Memory bandwidth and cache behavior also affect the result.

## Conclusion
Both parallel methods should match the serial result. Compare the measured timings to see whether the extra iteration-level flexibility from `collapse(2)` is useful for each matrix size on your system.
