# Task 3 – Histogram Calculation

Input values are in the range 0–9. Each thread updates a private array containing ten counters; therefore, threads never concurrently update the same counter. After the parallel region, the private histograms are summed into the final histogram. This avoids both a data race and the heavy contention of a critical section around every increment. Correctness checks compare all ten bins with a serial histogram and verify that the counts sum to the input length.
