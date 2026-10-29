
// Prevent this header file from being included more than once in the
// same compilation unit.
//
// #pragma once tells the compiler to process this file only once,
// avoiding duplicate definitions when multiple files include common.h.
#pragma once

// Provides standard algorithms such as std::min(), std::equal(),
// and other general-purpose operations.
#include <algorithm>

// Provides mathematical functions and constants.
#include <cmath>

// Provides formatting tools for controlling the appearance of output,
// such as the number of decimal places displayed.
#include <iomanip>

// Provides standard input and output facilities, including std::cout.
#include <iostream>

// Provides the std::vector container, which stores a sequence of elements
// whose size can be determined or changed dynamically.
#include <vector>

// Provides OpenMP functions, including omp_get_wtime() for measuring
// elapsed wall-clock time and omp_set_dynamic() for controlling
// automatic adjustment of the number of threads.
#include <omp.h>

// Print the shared column headings used by all three experiments.
//
// This function displays the column names for the performance results.
// The same function can be called by different tasks so that their
// result tables use a consistent format.
//
// inline allows this function definition to appear in a header included
// by multiple source files without causing multiple-definition errors.
inline void header() {

    // Print the first column heading: Threads.
    //
    // std::left aligns the text to the left.
    // std::setw(9) sets the minimum field width to 9 characters.
    std::cout << std::left << std::setw(9) << "Threads"

              // Align the next heading to the right and reserve
              // 16 character positions for the execution time.
              << std::right << std::setw(16) << "Time (s)"

              // Reserve 13 character positions for the speedup value.
              << std::setw(13) << "Speedup"

              // Reserve 15 character positions for the efficiency value.
              << std::setw(15) << "Efficiency"

              // Reserve 12 character positions for the correctness check.
              << std::setw(12) << "Check" << '\n'

              // Print a horizontal separator made of 65 hyphens.
              // This separates the table headings from the result rows.
              << std::string(65, '-') << '\n';
}

// Print one row of performance results.
//
// Parameters:
// threads - Number of OpenMP threads used for the experiment.
// time    - Execution time measured for the parallel implementation.
// serial  - Execution time measured for the sequential implementation.
// ok      - Whether the parallel result passed the correctness check.
//
// Speedup measures how many times faster the parallel execution is
// compared with the sequential execution.
//
// Speedup = serial execution time / parallel execution time
//
// Efficiency measures how effectively the requested threads are used.
//
// Efficiency = speedup / number of threads
//
// PASS indicates that the parallel result matches the serial reference.
// FAIL indicates that the correctness check did not succeed.
inline void row(int threads, double time, double serial, bool ok) {

    // Calculate speedup.
    //
    // If the parallel execution time is greater than zero, divide the
    // serial time by the parallel time.
    //
    // The conditional expression prevents division by zero. If time is
    // zero or negative, speedup is set to zero.
    const double speedup = time > 0 ? serial / time : 0;

    // Calculate parallel efficiency by dividing speedup by the number
    // of threads used in the experiment.
    //
    // For example, if speedup is 4 and threads is 4, efficiency is 1.
    // This corresponds to 100% efficiency when expressed as a percentage.
    //
    // The value is printed as a ratio, not multiplied by 100.
    const double efficiency = speedup / threads;

    // Print the number of threads in the first column.
    // std::left aligns the value to the left within a field of width 9.
    std::cout << std::left << std::setw(9) << threads

              // Align the remaining output columns to the right.
              // std::fixed displays floating-point numbers in fixed-point
              // notation rather than scientific notation.
              //
              // std::setprecision(6) specifies six digits after the
              // decimal point for the execution time.
              << std::right << std::fixed << std::setprecision(6)

              // Print the measured execution time in seconds.
              // The field width is 16 characters.
              << std::setw(16) << time

              // Change the floating-point precision to three digits
              // after the decimal point for the speedup value.
              //
              // This precision setting remains active for subsequent
              // floating-point output until it is changed again.
              << std::setprecision(3) << std::setw(13) << speedup

              // Print the calculated efficiency using the same precision
              // of three digits after the decimal point.
              << std::setw(15) << efficiency

              // Print the correctness status.
              //
              // If ok is true, display PASS.
              // Otherwise, display FAIL.
              //
              // The final '\n' moves the output to the next line.
              << std::setw(12) << (ok ? "PASS" : "FAIL") << '\n';
}

// Measure the elapsed wall-clock time of an operation.
//
// This is a function template, so it can accept different callable objects,
// such as lambda expressions, without requiring a separate timing function
// for each task.
//
// Template parameter F represents the type of the operation being passed.
//
// The function returns a double representing the elapsed time in seconds.
template <class F>
double timed(F&& operation) {

    // Record the starting wall-clock time.
    //
    // omp_get_wtime() returns a wall-clock timestamp in seconds.
    // It is useful for measuring elapsed time in OpenMP programs.
    //
    // The returned value is a timestamp, not the duration of the operation.
    const double start = omp_get_wtime();

    // Execute the operation supplied by the caller.
    //
    // For example, a task can pass a lambda containing a serial loop
    // or a parallel OpenMP loop.
    //
    // The function executes that operation before measuring the ending
    // time. Any work performed inside the operation contributes to the
    // elapsed time.
    operation();

    // Read the wall-clock time again and subtract the starting timestamp.
    //
    // The difference gives the elapsed time in seconds.
    //
    // The function then returns this duration to the calling code.
    return omp_get_wtime() - start;
}

