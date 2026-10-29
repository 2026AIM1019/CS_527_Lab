#ifndef CS527_OPENMP_LAB8_COMMON_H
#define CS527_OPENMP_LAB8_COMMON_H

// Provides functions such as sort() used to sort timing values.
#include <algorithm>

// Provides time-related utilities.
#include <chrono>

// Provides the function wrapper used by the timing template.
#include <functional>

// Provides output formatting tools such as setw() and setprecision().
#include <iomanip>

// Provides cout and other input/output operations.
#include <iostream>

// Provides numeric operations such as accumulate().
#include <numeric>

// Provides the string data type.
#include <string>

// Provides the vector container used to store values.
#include <vector>

// Provides OpenMP functions, including omp_get_wtime().
#include <omp.h>

// Allows standard library names to be used without writing std:: each time.
using namespace std;

// Returns the median value from a collection of execution times.
// The median is used because it is less affected by unusually high or low timings.
inline double median_seconds(const vector<double>& values) {
    // Make a copy so the original timing values are not modified.
    vector<double> sorted = values;

    // Arrange the copied timing values in ascending order.
    sort(sorted.begin(), sorted.end());

    // Return the middle value of the sorted collection.
    // For an even number of values, this returns the element at size / 2.
    return sorted[sorted.size() / 2];
}

// Measures how long a function takes to execute and returns its median time.
// Function is a template parameter, so this function can accept different
// callable objects, such as lambda functions.
template <typename Function>
inline double measure_median(Function function, int repeats = 3) {
    // Execute the function once before collecting timing samples.
    // This initial execution is not included in the measured samples.
    // It can help reduce the effect of initial setup costs.
    function();

    // Stores the execution time recorded during each measured run.
    vector<double> samples;

    // Reserve enough space for the requested number of timing samples.
    samples.reserve(repeats);

    // Execute the function repeatedly to collect timing measurements.
    for (int i = 0; i < repeats; ++i) {
        // Record the starting time in seconds.
        const double start = omp_get_wtime();

        // Execute the function whose performance is being measured.
        function();

        // Calculate the elapsed time and store it in the samples vector.
        samples.push_back(omp_get_wtime() - start);
    }

    // Return the median of the collected execution times.
    return median_seconds(samples);
}

// Prints the column headings for a performance results table.
// time_name allows the execution-time column to have a custom heading.
// By default, the heading is "Parallel (s)".
inline void print_timing_header(const string& time_name = "Parallel (s)") {
    // Print the thread count column heading, aligned to the left.
    cout << left << setw(9) << "Threads"
              // Print the execution-time column heading, aligned to the right.
              << right << setw(16) << time_name
              // Print the speedup column heading.
              << setw(14) << "Speedup"
              // Print the parallel efficiency column heading.
              << setw(14) << "Efficiency"
              // Print the correctness-check column heading.
              << setw(14) << "Check" << '\n';

    // Print a line of 67 hyphens below the headings.
    cout << string(67, '-') << '\n';
}

// Prints one row of performance results.
// threads is the number of threads used by the parallel implementation.
// seconds is the measured parallel execution time.
// serial_seconds is the measured serial execution time.
// pass indicates whether the parallel result passed its correctness check.
inline void print_timing_row(int threads, double seconds, double serial_seconds, bool pass) {
    // Calculate speedup as serial time divided by parallel time.
    // Avoid division by zero if the parallel execution time is zero.
    const double speedup = seconds > 0.0 ? serial_seconds / seconds : 0.0;

    // Calculate parallel efficiency by dividing speedup by thread count.
    // An efficiency of 1.0 corresponds to 100% efficiency.
    const double efficiency = speedup / threads;
    
    // Print the number of threads used for this measurement.
    cout << left << setw(9) << threads
              // Use right alignment and fixed-point decimal formatting.
              // Print the execution time with nine digits after the decimal.
              << right << fixed << setprecision(9)
              << setw(16) << seconds
              // Print speedup with three digits after the decimal.
              << setw(14) << setprecision(3) << speedup
              // Print the calculated efficiency.
              << setw(14) << efficiency
              // Print PASS if the correctness check succeeded; otherwise FAIL.
              << setw(14) << (pass ? "PASS" : "FAIL") << '\n';
}

// Returns the list of thread counts used for performance testing.
// The static vector is created only once and reused on subsequent calls.
// Returning a reference avoids copying the vector every time this function runs.
inline const vector<int>& thread_counts() {
    // Test the algorithms using 1, 2, 4, and 8 threads.
    static const vector<int> counts{1, 2, 4, 8};

    // Return a constant reference to the stored thread counts.
    // The const reference prevents callers from modifying the vector.
    return counts;
}

// Function declarations for the five OpenMP lab tasks.
// These declarations tell the compiler that the functions exist.
// Their implementations are written in the corresponding task source files.

// Runs Task 1.
void run_task1();

// Runs Task 2.
void run_task2();

// Runs Task 3.
void run_task3();

// Runs Task 4.
void run_task4();

// Runs Task 5.
void run_task5();

// Ends the include guard.
// It prevents the contents of this header from being included multiple times
// in the same compilation unit.
#endif
