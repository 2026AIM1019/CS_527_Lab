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

Here is the GitHub-ready Markdown for Task 5: Naive Matrix-Matrix Multiplication, including all benchmark results, speedup, efficiency, correctness checks, and observations.

Writing

# Results to Record

> **Note:** Matrices use flattened row-major storage. Each element of matrix `C` is written once per output element. Timings are medians of 3 runs after a warm-up. The `N = 1024` benchmark can take several minutes.

## Matrix Size: 256 × 256

**Serial Median Time:** `0.038211947 s`

### Parallel Performance

| Threads | Outer Time (s) | Outer Speedup | Outer Efficiency | Outer Check | Collapse Time (s) | Collapse Speedup | Collapse Efficiency | Collapse Check |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 0.041114783 | 0.929× | 0.929 | PASS | 0.037508491 | 1.019× | 1.019 | PASS |
| 2 | 0.019757866 | 1.934× | 0.967 | PASS | 0.020726689 | 1.844× | 0.922 | PASS |
| 4 | 0.018558245 | 2.059× | 0.515 | PASS | 0.021100289 | 1.811× | 0.453 | PASS |
| 8 | 0.020246450 | 1.887× | 0.236 | PASS | 0.020151738 | 1.896× | 0.237 | PASS |

## Matrix Size: 512 × 512

**Serial Median Time:** `0.599942949 s`

### Parallel Performance

| Threads | Outer Time (s) | Outer Speedup | Outer Efficiency | Outer Check | Collapse Time (s) | Collapse Speedup | Collapse Efficiency | Collapse Check |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 0.723076724 | 0.830× | 0.830 | PASS | 0.751141028 | 0.799× | 0.799 | PASS |
| 2 | 0.358860517 | 1.672× | 0.836 | PASS | 0.389414873 | 1.541× | 0.770 | PASS |
| 4 | 0.738985171 | 0.812× | 0.203 | PASS | 0.567973732 | 1.056× | 0.264 | PASS |
| 8 | 0.440895149 | 1.361× | 0.170 | PASS | 0.455294079 | 1.318× | 0.165 | PASS |

## Matrix Size: 1024 × 1024

**Serial Median Time:** `13.052832604 s`

### Parallel Performance

| Threads | Outer Time (s) | Outer Speedup | Outer Efficiency | Outer Check | Collapse Time (s) | Collapse Speedup | Collapse Efficiency | Collapse Check |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 11.673869361 | 1.118× | 1.118 | PASS | 11.698154401 | 1.116× | 1.116 | PASS |
| 2 | 7.031100796 | 1.856× | 0.928 | PASS | 6.142095453 | 2.125× | 1.063 | PASS |
| 4 | 9.066915328 | 1.440× | 0.360 | PASS | 5.170357380 | 2.525× | 0.631 | PASS |
| 8 | 6.296820628 | 2.073× | 0.259 | PASS | 5.455645059 | 2.393× | 0.299 | PASS |

## Summary and Observations

- **Correctness:** All benchmark configurations passed their correctness checks.
- **Best performance for 256 × 256:** The outer-loop implementation with 4 threads was fastest at `0.018558245 s`, achieving a speedup of `2.059×`.
- **Best performance for 512 × 512:** The outer-loop implementation with 2 threads was fastest at `0.358860517 s`, achieving a speedup of `1.672×`.
- **Best performance for 1024 × 1024:** The collapsed-loop implementation with 4 threads was fastest at `5.170357380 s`, achieving a speedup of `2.525×`.
- **Outer-loop parallelization:** Parallelizing the outer loop distributes independent rows of the output matrix among threads.
- **Collapsed-loop parallelization:** Collapsing the outer loops exposes more iterations to parallel execution and can improve load distribution across threads.
- **Thread scaling:** Increasing the thread count does not always improve performance. Scheduling overhead, cache behavior, memory access patterns, and available computational resources can affect scaling.
- **Sequential inner loop:** The `k` loop remains sequential because its iterations contribute to the same output element. Parallelizing the outer loops avoids concurrent writes to an individual output element.

## Conclusion

Naive matrix-matrix multiplication benefits from parallelizing independent output elements. Both outer-loop and collapsed-loop implementations improve performance, although their relative effectiveness depends on matrix size and thread count.

For the largest tested matrix, the collapsed-loop implementation with 4 threads achieves the best observed performance, with a speedup of `2.525×`. However, efficiency decreases as the number of threads increases, demonstrating that additional threads do not necessarily translate into proportional performance gains.

**Key takeaway:** Choosing the appropriate parallel loop structure and thread count is essential for obtaining good performance in matrix multiplication.

## Questions and answers

### 1. Why is it safe to parallelize the i loop?
Each `i` iteration computes a different row of `C`. Threads assigned different rows write to different output elements. The `k` summation for each particular `C[i][j]` remains inside one thread, so there is no concurrent update to that output element.

### 2. What if the k loop is parallelized with a plain `parallel for`?
Different `k` iterations contribute terms to the same `C[i][j]`. If they concurrently execute `C[i][j] += A[i][k] * B[k][j]`, their read-modify-write operations can race and lose updates. Correctly parallelizing `k` would require a reduction or separate partial sums followed by a combine step. The current implementation avoids this issue by keeping `k` sequential.

### 3. When does `collapse(2)` matter most?
Parallelizing only `i` exposes `N` independent outer-loop iterations. `collapse(2)` exposes `N*N` independent `(i,j)` output elements. It is most useful when `N` is too small to provide enough outer-loop iterations for all threads. At the assignment's larger sizes, both approaches expose ample parallel work, so `collapse(2)` may have little benefit or may add scheduling overhead.

## Conclusion
Use actual measurements to compare scaling. Matrix multiplication has much more computation than matrix addition, but its performance is also affected by memory access patterns, cache behavior, and available CPU resources. The optional `i,k,j` loop order can improve locality because it accesses rows of `B` and `C` more sequentially, but this report's required baseline uses `i,j,k`.
