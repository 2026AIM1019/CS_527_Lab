# Task 2 Report — Sum of Array Elements

## Objective
Compute the sum of an integer array, observe why an unsynchronized parallel sum is unsafe, and use OpenMP's `reduction` clause to obtain a correct parallel result.

## Implementation
- `sumSerial()` calculates the reference result sequentially.
- `sumNaive()` intentionally updates one shared `sum` from multiple threads without synchronization.
- `sumReduction()` uses `#pragma omp parallel for reduction(+:sum)`.
- Sizes: `10^7` and `10^8`; thread counts: 1, 2, 4, 8. The naive version is run three times per thread count to make its inconsistent behavior easier to observe.

## Correctness and data race
In the naive version, `sum += a[i]` is a read-modify-write operation. Two threads may read the same old value of `sum`, add different array values, and then write back; one update can overwrite the other. This is a data race, so the program's result is undefined. It might be wrong, inconsistent between runs, or occasionally appear correct. With `reduction`, OpenMP creates a private partial sum for each thread and combines the partial sums at the end.

## Timing methodology
The serial and reduction timings are reported as medians of three measured runs after a warm-up. The intentionally unsafe naive implementation is executed three times for each thread count, and all three results are printed. Because a data race is undefined behavior, these runs are for observation only and are not correctness tests.

Here is the GitHub-ready Markdown for Task 2: Sum of Array Elements, with properly formatted tables and a summary.

Writing

# Result

> **Important:** The naive sum is intentionally data-racy. Its results are observations only and must never be treated as valid answers. Reduction timings are medians of 3 runs after a warm-up.

## N = 10,000,000

**Serial Sum:** `59,999,995`\
**Serial Median Time:** `0.009299292 s`

### Naive Sum vs. Reduction

| Threads | Naive Run 1 | Naive Run 2 | Naive Run 3 | Naive Median (s) | Reduction Sum | Reduction Time (s) | Check |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 59,999,995 | 59,999,995 | 59,999,995 | 0.007714525 | 59,999,995 | 0.008563712 | PASS |
| 2 | 29,999,985 | 29,999,985 | 30,000,010 | 0.014533442 | 59,999,995 | 0.005784226 | PASS |
| 4 | 14,999,997 | 15,000,006 | 14,999,988 | 0.006626891 | 59,999,995 | 0.004621011 | PASS |
| 8 | 22,499,990 | 15,000,009 | 7,499,990 | 0.006331932 | 59,999,995 | 0.005010353 | PASS |

## N = 100,000,000

**Serial Sum:** `599,999,995`\
**Serial Median Time:** `0.079149677 s`

### Naive Sum vs. Reduction

| Threads | Naive Run 1 | Naive Run 2 | Naive Run 3 | Naive Median (s) | Reduction Sum | Reduction Time (s) | Check |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 599,999,995 | 599,999,995 | 599,999,995 | 0.118476403 | 599,999,995 | 0.096855192 | PASS |
| 2 | 300,000,010 | 300,000,010 | 300,000,010 | 0.054888639 | 599,999,995 | 0.055094925 | PASS |
| 4 | 150,000,006 | 150,000,006 | 150,000,004 | 0.057168907 | 599,999,995 | 0.055445302 | PASS |
| 8 | 75,000,010 | 75,000,010 | 75,000,000 | 0.062390876 | 599,999,995 | 0.075095051 | PASS |

## Summary and Observations

- **Correctness:** The reduction produces the expected sum for all tested thread counts, and every reported correctness check passes.
- **Naive sum:** The results vary with thread count because concurrent updates introduce a data race. These values are not reliable answers, even when they happen to match the serial sum.
- **Best reduction performance for N = 10,000,000:** 4 threads, with a time of `0.004621011 s`.
- **Best reduction performance for N = 100,000,000:** 2 threads, with a time of `0.055094925 s`.
- **Thread scaling:** Increasing the number of threads does not always improve performance. Synchronization overhead, memory bandwidth, and scheduling can limit the benefits of parallel execution.
- **Critical section alternative:** Protecting every sum update with a critical section can prevent concurrent updates from interfering with one another, but it serializes access to the shared sum and is usually slower than a reduction.

## Conclusion

Parallel reduction is the preferred approach for computing the sum of array elements safely and efficiently. It avoids the data race of the naive implementation while allowing threads to accumulate partial sums independently before combining them into the final result. The optimal thread count depends on the input size and the hardware.

## Questions and answers

### 1. Why does the naive version give wrong or inconsistent answers?
All threads update the same shared variable without protection. Their read-modify-write operations can overlap, causing lost updates. This is the data race. A particular run may coincidentally produce the correct result, so one successful run does not make the code safe.

### 2. Would `critical` fix correctness? What about performance?
Yes. Putting each update inside `#pragma omp critical` would allow only one thread at a time to modify `sum`, protecting the update. However, every element update would have to wait for the same critical section. That contention serializes much of the work and can make it slower than the serial version. `reduction` is generally better because each thread accumulates locally and only partial results need to be combined.

## Conclusion
The reduction implementation should match the serial sum on every run. The naive version is deliberately unsafe and its output must not be used as a valid answer. Record actual measurements from the machine used for testing.
