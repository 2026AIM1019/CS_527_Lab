#ifndef CS527_OPENMP_LAB8_COMMON_H
#define CS527_OPENMP_LAB8_COMMON_H

#include <algorithm>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <omp.h>

using namespace std;

inline double median_seconds(const vector<double>& values) {
    vector<double> sorted = values;

    sort(sorted.begin(), sorted.end());

    return sorted[sorted.size() / 2];
}

template <typename Function>
inline double measure_median(Function function, int repeats = 3) {
    function();
    vector<double> samples;

    samples.reserve(repeats);

    for (int i = 0; i < repeats; ++i) {
        const double start = omp_get_wtime();
        function();
        samples.push_back(omp_get_wtime() - start);
    }

    return median_seconds(samples);
}

inline void print_timing_header(const string& time_name = "Parallel (s)") {
    cout << left << setw(9) << "Threads"
              << right << setw(16) << time_name
              << setw(14) << "Speedup"
              << setw(14) << "Efficiency"
              << setw(14) << "Check" << '\n';

    cout << string(67, '-') << '\n';
}

inline void print_timing_row(int threads, double seconds, double serial_seconds, bool pass) {
    const double speedup = seconds > 0.0 ? serial_seconds / seconds : 0.0;
    const double efficiency = speedup / threads;
    
    cout << left << setw(9) << threads
              << right << fixed << setprecision(9)
              << setw(16) << seconds
              << setw(14) << setprecision(3) << speedup
              << setw(14) << efficiency
              << setw(14) << (pass ? "PASS" : "FAIL") << '\n';
}

inline const vector<int>& thread_counts() {
    static const vector<int> counts{1, 2, 4, 8};
    return counts;
}

void run_task1();
void run_task2();
void run_task3();
void run_task4();
void run_task5();

#endif
