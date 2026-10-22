# Task 2 – Parallel Array Sorting

A recursive merge sort is parallelized using OpenMP tasks. The two recursive halves operate on disjoint index ranges. `taskwait` is essential: it prevents the parent from merging before both halves are sorted. The merge writes to the corresponding disjoint range in a shared temporary array, so no critical section is necessary. Small ranges use `sort` to avoid excessive task overhead. Correctness is checked against a serial `sort` result and with `is_sorted`.
