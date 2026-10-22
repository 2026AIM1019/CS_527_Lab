# Task 3 Report — Maximum and Minimum of an Array

## Objective
Find the maximum and minimum values in an integer array and identify indices containing those values. Compare value reductions, a critical-section index search, and a manual per-thread index reduction.

## Implementation
- `serialMinMax()` finds reference values and indices in one serial pass.
- `parallelValues()` uses `reduction(max:maxValue)` and `reduction(min:minValue)` to find the values.
- `indicesCritical()` updates a shared best value/index pair inside `#pragma omp critical`.
- `indicesManual()` lets each thread track its own local maximum/minimum and indices, then combines those local results after the parallel region.
- Sizes: `10^7` and `10^8`; thread counts: 1, 2, 4, 8. Timing uses `omp_get_wtime()`.

## Correctness notes
A reduction on the value alone does not automatically return the corresponding index. The index is a second piece of information that must be kept paired with the value. The program verifies both the value and the index against the serial result. To make results deterministic, all implementations select the smallest index when multiple elements share an extremal value.

## Timing methodology
The serial, value-reduction, and manual-index timings use a warm-up followed by three measured runs; the median is reported. The critical-section index method also uses three measured runs, although it can take substantially longer because the critical section is entered during the scan. Correctness checks verify both extremal values and their exact indices. Ties are resolved by choosing the smallest index.

Here is the GitHub-ready Markdown for Task 3: Maximum and Minimum of an Array, with formatted benchmark tables and a summary of the results.

Writing

# Results to Record

> **Note:** Timings are medians of 3 runs after a warm-up. When multiple elements share the same maximum or minimum value, the smallest index is selected.

## N = 10,000,000

**Serial Results:**

- **Maximum:** `500001` at index `594596`
- **Minimum:** `-500001` at index `837840`
- **Serial Median Time:** `0.011910713 s`

### Parallel Performance

| Threads | Values Time (s) | Critical Index Time (s) | Manual Index Time (s) | Check |
| --- | --- | --- | --- | --- |
| 1 | 0.008465151 | 0.190721084 | 0.012363846 | PASS |
| 2 | 0.005897642 | 0.966114143 | 0.008010188 | PASS |
| 4 | 0.013409302 | 0.812688505 | 0.022858952 | PASS |
| 8 | 0.016643154 | 0.396343939 | 0.029341840 | PASS |

## N = 100,000,000

**Serial Results:**

- **Maximum:** `500001` at index `594596`
- **Minimum:** `-500001` at index `837840`
- **Serial Median Time:** `0.176327331 s`

### Parallel Performance

| Threads | Values Time (s) | Critical Index Time (s) | Manual Index Time (s) | Check |
| --- | --- | --- | --- | --- |
| 1 | 0.121947761 | 2.192415261 | 0.171279756 | PASS |
| 2 | 0.053414666 | 5.828544580 | 0.125744374 | PASS |
| 4 | 0.085742016 | 7.865658107 | 0.080294624 | PASS |
| 8 | 0.056220007 | 8.178056330 | 0.088484566 | PASS |

## Summary and Observations

- **Correctness:** All configurations passed the checks for the maximum and minimum values and the exact smallest-index tie-breaking rule.
- **Best performance for N = 10,000,000:** The values approach with 2 threads was the fastest, taking `0.005897642 s`.
- **Best performance for N = 100,000,000:** The values approach with 2 threads was the fastest, taking `0.053414666 s`.
- **Critical index approach:** This approach is significantly slower because critical sections serialize access to shared resources, creating contention between threads.
- **Manual index approach:** Performance varies with input size and thread count. It can avoid the heavy synchronization costs of critical sections, but its efficiency depends on how the implementation computes and combines local extrema indices.
- **Scalability:** Increasing the thread count does not guarantee better performance. Synchronization overhead, memory bandwidth, and scheduling can affect execution time.

## Conclusion

Parallel computation of maximum and minimum values can significantly reduce execution time for large arrays. The values approach performs best in these benchmarks, while the critical index approach suffers from substantial synchronization overhead.

Correctness requires more than finding the maximum and minimum values: when ties occur, the implementation must also return the smallest corresponding index. All tested configurations satisfy this requirement.

## Questions and answers

### 1. Why cannot `reduction(max:best_idx)` find the index of the maximum?
It would find the largest numeric index, not the index associated with the largest array value. The maximum value and its index must be compared and combined as a pair.

### 2. Which index method is faster, and why?
The manual per-thread method is generally expected to be faster because each thread works on local state and only a small number of local results are combined at the end. The critical-section method can force threads to wait for one another while they compare/update the shared pair. Actual timings should be used to confirm the trend on the test machine.

## Conclusion
The value reductions and both index methods are checked against the serial answer. Use the measured timing table to compare the synchronization cost of the critical-section approach with the local-work/final-combine approach.
