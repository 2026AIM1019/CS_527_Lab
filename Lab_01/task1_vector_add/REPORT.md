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

| N | Serial time (s) | Threads | Parallel time (s) | Correct? | Speedup (serial / parallel) |
|---:|---:|---:|---:|:---:|---:|
| 100,000 | ___ | 1 | ___ | ___ | ___ |
| 100,000 | ___ | 2 | ___ | ___ | ___ |
| 100,000 | ___ | 4 | ___ | ___ | ___ |
| 100,000 | ___ | 8 | ___ | ___ | ___ |
| 1,000,000 | ___ | 1 | ___ | ___ | ___ |
| 1,000,000 | ___ | 2 | ___ | ___ | ___ |
| 1,000,000 | ___ | 4 | ___ | ___ | ___ |
| 1,000,000 | ___ | 8 | ___ | ___ | ___ |
| 10,000,000 | ___ | 1 | ___ | ___ | ___ |
| 10,000,000 | ___ | 2 | ___ | ___ | ___ |
| 10,000,000 | ___ | 4 | ___ | ___ | ___ |
| 10,000,000 | ___ | 8 | ___ | ___ | ___ |
| 100,000,000 | ___ | 1 | ___ | ___ | ___ |
| 100,000,000 | ___ | 2 | ___ | ___ | ___ |
| 100,000,000 | ___ | 4 | ___ | ___ | ___ |
| 100,000,000 | ___ | 8 | ___ | ___ | ___ |

## Questions and answers

### 1. At what size does parallelization start paying off? Why is it not beneficial for small N?
Use the measurements to identify the crossover point on your machine. For small arrays, creating/scheduling the OpenMP team and coordinating threads can cost more time than the additions themselves. For larger arrays, the work is sufficient to offset that overhead. The exact crossover depends on the system and runtime.

### 2. Does speedup increase linearly with thread count?
Usually not. Memory bandwidth, thread-management overhead, available physical cores, cache behavior, and operating-system scheduling limit scaling. Vector addition performs very little arithmetic per byte moved, so memory bandwidth often becomes the bottleneck.

## Conclusion
The experiment demonstrates how a loop with independent iterations can be parallelized. Fill in the tables with measured output and state the observed crossover size and scaling trend for your machine.
