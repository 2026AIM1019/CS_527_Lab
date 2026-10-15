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

## Results to record
| N | Threads | Serial time (s) | Value reduction (s) | Critical indices (s) | Manual indices (s) | Correct? |
|---:|---:|---:|---:|---:|---:|:---:|
| 10,000,000 | 1 | ___ | ___ | ___ | ___ | ___ |
| 10,000,000 | 2 | ___ | ___ | ___ | ___ | ___ |
| 10,000,000 | 4 | ___ | ___ | ___ | ___ | ___ |
| 10,000,000 | 8 | ___ | ___ | ___ | ___ | ___ |
| 100,000,000 | 1 | ___ | ___ | ___ | ___ | ___ |
| 100,000,000 | 2 | ___ | ___ | ___ | ___ | ___ |
| 100,000,000 | 4 | ___ | ___ | ___ | ___ | ___ |
| 100,000,000 | 8 | ___ | ___ | ___ | ___ | ___ |

## Questions and answers

### 1. Why cannot `reduction(max:best_idx)` find the index of the maximum?
It would find the largest numeric index, not the index associated with the largest array value. The maximum value and its index must be compared and combined as a pair.

### 2. Which index method is faster, and why?
The manual per-thread method is generally expected to be faster because each thread works on local state and only a small number of local results are combined at the end. The critical-section method can force threads to wait for one another while they compare/update the shared pair. Actual timings should be used to confirm the trend on the test machine.

## Conclusion
The value reductions and both index methods are checked against the serial answer. Use the measured timing table to compare the synchronization cost of the critical-section approach with the local-work/final-combine approach.
