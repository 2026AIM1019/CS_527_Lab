# Task 1 — Recursive OpenMP Task Merge Sort

## Objective and implementation

Implement serial merge sort and recursive task-parallel merge sort. Each split produces two disjoint subarrays. The parent creates two OpenMP tasks, waits for both, then merges the sorted halves. A single `std::vector<int>` temporary array is allocated before sorting; child tasks write only within their own index ranges. The parallel algorithm is launched inside an OpenMP `parallel` region with one `single` initial task.

Array sizes: 10^5, 10^6, 10^7. Threads: 1, 2, 4, 8. The task cutoff is 4096 elements. Values are generated deterministically; each parallel result must be sorted and exactly equal to the serial merge-sort output. The table reports elapsed seconds, speedup, efficiency, and PASS/FAIL.

## Questions

**Tasks versus parallel for:** `parallel for` divides the iterations of a known loop among threads; `task` creates units of work that can be executed by any team thread. Tasks are useful for recursive divide-and-conquer problems where work is discovered dynamically.

**Why taskwait?** Both children must finish sorting their halves before the parent merges them. Without `taskwait`, merging could read and overwrite data while a child is still sorting it, creating races and incorrect output.

**Cutoff effect:** A larger cutoff creates fewer tasks, reducing overhead but also limiting available parallelism. A smaller cutoff exposes more parallelism but creates many small tasks; 4096 is a reasonable illustrative value, not a universal optimum.

**Why slower for small arrays?** Thread scheduling, task creation, synchronization, and merging overhead can dominate the small amount of useful sorting work.

**Task creation overhead and scalability:** Creating and scheduling too many tasks increases runtime overhead and contention. Performance also becomes limited by merge operations, memory bandwidth, and the serial portion of the computation.

## Interpretation

Use the output table to compare 1/2/4/8 threads for each N. Speedup need not grow linearly with thread count. Exact timings are machine dependent; verify all rows show PASS.
