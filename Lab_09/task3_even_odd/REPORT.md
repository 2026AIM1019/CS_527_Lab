# Task 3 — Multiple Reductions and Atomic Counting

## Objective and implementation

Count even and odd integers with a serial baseline, an OpenMP `reduction(+:even,odd)` loop, and an atomic-update implementation. Array sizes: 10^5, 10^6, 10^7, 10^8; threads: 1, 2, 4, 8. For each result, check both counts against the serial baseline and verify `even + odd == N`. Times, speedup, and efficiency are printed in aligned tables.

**Important benchmark distinction:** The atomic implementation first counts elements in private 4096-element blocks, then performs **two atomic updates per block**. It demonstrates safe atomic updates without incurring the extremely high cost of two atomics for each of 100 million elements. It does not measure the performance of per-element atomic increments, and should be labeled as *block-aggregated atomic* rather than plain per-element atomic.

## Questions

**Multiple variables in a reduction:** `#pragma omp parallel for reduction(+:even,odd)` gives each thread private copies of both counters, then combines them using addition at the end.

**Why reduction is usually faster:** Per-element atomic increments force synchronization on shared counters and cause contention; reductions keep updates local until the final combination. This experiment uses block aggregation for atomics, reducing their synchronization overhead and making the comparison less extreme.

**Why even + odd = N?** Every integer is either even or odd, never both. Each element contributes to exactly one category.

**Do more threads always help?** No. Thread overhead, memory bandwidth, contention, and the amount of work per thread can limit speedup or make additional threads slower.

## Interpretation

Check all rows for PASS. At 10^8 elements, the input array alone uses approximately 400 MB of RAM. Exact performance depends on machine resources and system load.
