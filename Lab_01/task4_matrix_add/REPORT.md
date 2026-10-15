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

## Results to record
| N | Threads | Serial time (s) | Outer-loop time (s) | Outer check | `collapse(2)` time (s) | Collapse check |
|---:|---:|---:|---:|:---:|---:|:---:|
| 8 | 1 | ___ | ___ | ___ | ___ | ___ |
| 8 | 2 | ___ | ___ | ___ | ___ | ___ |
| 8 | 4 | ___ | ___ | ___ | ___ | ___ |
| 8 | 8 | ___ | ___ | ___ | ___ | ___ |
| 4096 | 1 | ___ | ___ | ___ | ___ | ___ |
| 4096 | 2 | ___ | ___ | ___ | ___ | ___ |
| 4096 | 4 | ___ | ___ | ___ | ___ | ___ |
| 4096 | 8 | ___ | ___ | ___ | ___ | ___ |

## Questions and answers

### 1. Does `collapse(2)` help for small N?
For `N = 8`, the outer loop has only eight iterations. With more threads than useful outer-loop iterations, some threads may receive no row to process. `collapse(2)` exposes 64 `(i,j)` iterations, giving the runtime more opportunities to distribute work. For such a small matrix, however, thread startup and scheduling overhead can outweigh the saved work, so a lower time is not guaranteed.

### 2. Does `collapse(2)` matter for large N?
For `N = 4096`, the outer loop already has 4096 independent rows, which is enough work to keep 1, 2, 4, or 8 threads occupied. `collapse(2)` may therefore offer little improvement and can add scheduling overhead. Memory bandwidth and cache behavior also affect the result.

## Conclusion
Both parallel methods should match the serial result. Compare the measured timings to see whether the extra iteration-level flexibility from `collapse(2)` is useful for each matrix size on your system.
