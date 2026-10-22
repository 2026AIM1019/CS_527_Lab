# Task 4 – Matrix Transpose

For a row-major matrix `A` with `rows × columns` elements, the transpose satisfies `T[column * rows + row] = A[row * columns + column]`. The parallel loop uses `collapse(2)` to distribute independent row/column pairs. Each pair writes to a unique output index, so no synchronization is needed. The full output is compared with a serial transpose.
