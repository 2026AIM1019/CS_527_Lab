# Lab 8 build and test results

## Build check

The project compiled successfully using:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -fopenmp \
  main.cpp task1_array_search/main.cpp task2_array_sort/main.cpp \
  task3_histogram/main.cpp task4_matrix_transpose/main.cpp \
  task5_convolution/main.cpp -o build/cs527_lab8
```

Each of the five task source files was also compiled individually to an object file with `-std=c++17 -O2 -Wall -Wextra -Wpedantic -fopenmp`; all five compiled without warnings or errors.

## Runtime checks

Each task was run through the menu with its benchmark sizes and thread counts 1, 2, 4, and 8. Across those runs, **64 correctness checks passed and zero checks failed**. The 5×5 convolution example also passed and produced:

```text
 70  80  90
120 130 140
170 180 190
```

The complete sample console outputs are included in `sample_output/`. Timings are from the machine used to run the tests; timing results will vary on other systems and are not guarantees of performance on the submission machine.
