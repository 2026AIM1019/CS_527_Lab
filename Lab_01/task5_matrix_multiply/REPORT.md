# Task 5 Report — Naive Matrix-Matrix Multiplication

## Objective
Compute `C = A x B` using the classic triple-loop algorithm. Compare parallelizing the outer `i` loop with applying `collapse(2)` to the `i` and `j` loops, while keeping the summation `k` loop sequential.

## Implementation
- Matrices use flattened row-major `std::vector<double>` storage.
- For each output element, the implementation accumulates the dot product in a local variable `sum` and writes `C[i][j]` once.
- `multiplySerial()` is the reference implementation.
- `multiplyOuter()` parallelizes the `i` loop.
- `multiplyCollapse()` uses `#pragma omp parallel for collapse(2)` for the `i` and `j` loops.
- Sizes: `N = 256, 512, 1024`; thread counts: 1, 2, 4, 8.
- Outputs are compared with a tolerance because floating-point arithmetic can produce small rounding differences.

## Timing methodology
Each implementation is warmed up and timed three times; the median is reported. Correctness checking is outside the timed region. The `1024 x 1024` naive triple-loop case can take several minutes because repeated measurements are intentionally used to reduce timing noise.

## Results to record
Speedup is `serial time / parallel time`. Parallel efficiency is `speedup / thread count`.

| N | Threads | Serial time (s) | Outer time (s) | Outer speedup | Outer efficiency | Collapse time (s) | Collapse speedup | Collapse efficiency | Correct? |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|:---:|
| 256 | 1 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 256 | 2 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 256 | 4 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 256 | 8 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 512 | 1 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 512 | 2 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 512 | 4 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 512 | 8 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 1024 | 1 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 1024 | 2 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 1024 | 4 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |
| 1024 | 8 | ___ | ___ | ___ | ___ | ___ | ___ | ___ | ___ |

## Questions and answers

### 1. Why is it safe to parallelize the i loop?
Each `i` iteration computes a different row of `C`. Threads assigned different rows write to different output elements. The `k` summation for each particular `C[i][j]` remains inside one thread, so there is no concurrent update to that output element.

### 2. What if the k loop is parallelized with a plain `parallel for`?
Different `k` iterations contribute terms to the same `C[i][j]`. If they concurrently execute `C[i][j] += A[i][k] * B[k][j]`, their read-modify-write operations can race and lose updates. Correctly parallelizing `k` would require a reduction or separate partial sums followed by a combine step. The current implementation avoids this issue by keeping `k` sequential.

### 3. When does `collapse(2)` matter most?
Parallelizing only `i` exposes `N` independent outer-loop iterations. `collapse(2)` exposes `N*N` independent `(i,j)` output elements. It is most useful when `N` is too small to provide enough outer-loop iterations for all threads. At the assignment's larger sizes, both approaches expose ample parallel work, so `collapse(2)` may have little benefit or may add scheduling overhead.

## Conclusion
Use actual measurements to compare scaling. Matrix multiplication has much more computation than matrix addition, but its performance is also affected by memory access patterns, cache behavior, and available CPU resources. The optional `i,k,j` loop order can improve locality because it accesses rows of `B` and `C` more sequentially, but this report's required baseline uses `i,j,k`.
