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

## Results to record
| N | Threads | Serial sum | Naive result (run 1 / 2 / 3) | Reduction result | Reduction time (s) | Correct reduction? |
|---:|---:|---:|---|---:|---:|:---:|
| 10,000,000 | 1 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 10,000,000 | 2 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 10,000,000 | 4 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 10,000,000 | 8 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 100,000,000 | 1 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 100,000,000 | 2 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 100,000,000 | 4 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |
| 100,000,000 | 8 | ___ | ___ / ___ / ___ | ___ | ___ | ___ |

## Questions and answers

### 1. Why does the naive version give wrong or inconsistent answers?
All threads update the same shared variable without protection. Their read-modify-write operations can overlap, causing lost updates. This is the data race. A particular run may coincidentally produce the correct result, so one successful run does not make the code safe.

### 2. Would `critical` fix correctness? What about performance?
Yes. Putting each update inside `#pragma omp critical` would allow only one thread at a time to modify `sum`, protecting the update. However, every element update would have to wait for the same critical section. That contention serializes much of the work and can make it slower than the serial version. `reduction` is generally better because each thread accumulates locally and only partial results need to be combined.

## Conclusion
The reduction implementation should match the serial sum on every run. The naive version is deliberately unsafe and its output must not be used as a valid answer. Record actual measurements from the machine used for testing.
