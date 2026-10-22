# Task 5 – 2D Convolution

The 3×3 averaging kernel contains nine ones, and each output is the sum of its corresponding 3×3 image neighborhood divided by 9. No padding is used, so a `rows × columns` input produces `(rows-2) × (columns-2)` output. The parallel loop distributes output coordinates using `collapse(2)`; each output pixel is independent, so there are no write conflicts. Floating-point results are checked against the serial implementation with a small tolerance. The assignment's 5×5 example is also checked against the expected 3×3 matrix:

```text
 70  80  90
120 130 140
170 180 190
```
