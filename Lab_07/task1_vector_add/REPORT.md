# Task 1 Report — Vector Addition

## Objective
Compute `c[i] = a[i] + b[i]` for two arrays of `double` values. Compare a serial loop with an OpenMP parallel loop and observe how execution time changes with array size and thread count.

## Implementation
- `addSerial()` processes all indices on one thread.
- `addParallel()` uses `#pragma omp parallel for`, which distributes loop iterations among OpenMP threads.
- The arrays are initialized with deterministic values. The serial and parallel outputs are checked element by element.
- Timing uses `omp_get_wtime()`.
- Sizes: `10^5`, `10^6`, `10^7`, `10^8`; thread counts: 1, 2, 4, 8.

## Correctness
Each output element is independent: thread(s) writing `c[i]` do not need to read or modify any other output element. Therefore, the loop can be parallelized without a data race. The program prints `PASS` when every computed element matches the expected sum.

## Timing methodology
Each timed implementation is warmed up once. The displayed time is the median of three measured runs. Correctness verification is performed outside the timed region. Results still vary with CPU load and memory bandwidth.

## Results to record
Copy the timing output from your own machine. Do not compare timings collected on different machines as if they were directly equivalent.

Copy and paste this into your GitHub `README.md` file. The tables will render correctly in GitHub Markdown.

Writing

# Performance Benchmark Results

## N = 100,000

**Serial Median Time:** `0.000517328 s`\
**Correctness Check:** PASS

| Threads | Parallel Time (s) | Check | Speedup |
| --- | --- | --- | --- |
| 1 | 0.000480612 | PASS | 1.076× |
| 2 | 0.000194981 | PASS | 2.653× |
| 4 | 0.000604564 | PASS | 0.856× |
| 8 | 0.000507312 | PASS | 1.020× |

## N = 1,000,000

**Serial Median Time:** `0.003 s`\
**Correctness Check:** PASS

| Threads | Parallel Time (s) | Check | Speedup |
| --- | --- | --- | --- |
| 1 | 0.003058180 | PASS | 0.934× |
| 2 | 0.003182990 | PASS | 0.897× |
| 4 | 0.005641044 | PASS | 0.506× |
| 8 | 0.003726034 | PASS | 0.766× |

## N = 10,000,000

**Serial Median Time:** `0.026 s`\
**Correctness Check:** PASS

| Threads | Parallel Time (s) | Check | Speedup |
| --- | --- | --- | --- |
| 1 | 0.026286978 | PASS | 0.986× |
| 2 | 0.024089987 | PASS | 1.075× |
| 4 | 0.022067707 | PASS | 1.174× |
| 8 | 0.021908005 | PASS | 1.183× |

## N = 100,000,000

**Serial Median Time:** `0.215 s`\
**Correctness Check:** PASS

| Threads | Parallel Time (s) | Check | Speedup |
| --- | --- | --- | --- |
| 1 | 0.224104421 | PASS | 0.958× |
| 2 | 0.255974220 | PASS | 0.839× |
| 4 | 0.191830040 | PASS | 1.120× |
| 8 | 0.221352437 | PASS | 0.970× |

## Summary

- **Correctness:** All benchmark configurations passed the correctness check.
- **Best observed speedup:** `2.653×` with 2 threads at `N = 100,000`.
- **Best speedup for N = 10,000,000:** `1.183×` with 8 threads.
- **Best speedup for N = 100,000,000:** `1.120×` with 4 threads.
- **Scalability:** Parallel performance varies with input size and thread count. Larger inputs show modest speedups, while smaller inputs may incur parallelization overhead.

**Note:** A speedup greater than `1.0×` indicates an improvement over the serial execution time, while a speedup below `1.0×` indicates slower execution.

## Questions and answers

### 1. At what size does parallelization start paying off? Why is it not beneficial for small N?
Use the measurements to identify the crossover point on your machine. For small arrays, creating/scheduling the OpenMP team and coordinating threads can cost more time than the additions themselves. For larger arrays, the work is sufficient to offset that overhead. The exact crossover depends on the system and runtime.

### 2. Does speedup increase linearly with thread count?
Usually not. Memory bandwidth, thread-management overhead, available physical cores, cache behavior, and operating-system scheduling limit scaling. Vector addition performs very little arithmetic per byte moved, so memory bandwidth often becomes the bottleneck.

## Conclusion
The experiment demonstrates how a loop with independent iterations can be parallelized. Fill in the tables with measured output and state the observed crossover size and scaling trend for your machine.
