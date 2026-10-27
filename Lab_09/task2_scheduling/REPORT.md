# Task 2 — OpenMP Scheduling Strategies

## Objective and implementation

Compare `schedule(static)`, `schedule(dynamic,100)`, and `schedule(guided)` on an uneven computational workload. Each iteration transforms one input double with a deterministic recurrence. The number of iterations varies from 12 to 110 depending on its 10,000-element band. Output is independent per array element; the serial and parallel versions use identical operations. Correctness checks compare the complete arrays exactly.

Array sizes: 10^5, 10^6, 10^7. Threads: 1, 2, 4, 8. Each strategy reports execution time, speedup, efficiency, and PASS/FAIL.

## Questions

**Static, dynamic, guided:** Static divides iterations into predetermined chunks with little scheduling overhead. Dynamic hands out chunks as threads finish, which can improve balance for uneven workloads. Guided assigns larger chunks initially and gradually reduces chunk sizes, aiming to balance overhead against flexibility.

**Best for uniform versus uneven workloads:** Static is usually strong for uniform iterations because its overhead is low. Dynamic or guided may be better for uneven workloads, especially when slow iterations cluster in certain regions. The fastest strategy is hardware- and workload-dependent.

**Why dynamic overhead?** Threads repeatedly request more work from a shared scheduler, adding synchronization and bookkeeping overhead.

**Chunk size:** Small chunks improve load balance but increase scheduling overhead. Large chunks reduce overhead but risk assigning a disproportionate amount of heavy work to one thread. The example uses dynamic chunk size 100; varying it can identify a better tradeoff.

## Interpretation

Use the output tables to identify the fastest strategy for each N and thread count; do not assume one strategy always wins. The workload is deliberately nonuniform, but its pattern is deterministic and may interact with how chunks are assigned.
