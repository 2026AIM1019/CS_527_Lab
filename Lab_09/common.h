#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>

// Print the shared column headings used by all three experiments.
inline void header() {
    std::cout << std::left << std::setw(9) << "Threads"
              << std::right << std::setw(16) << "Time (s)"
              << std::setw(13) << "Speedup"
              << std::setw(15) << "Efficiency"
              << std::setw(12) << "Check" << '\n'
              << std::string(65, '-') << '\n';
}

// Speedup = serial time / parallel time; efficiency = speedup / threads.
// PASS indicates that the parallel result matches the serial reference.
inline void row(int threads, double time, double serial, bool ok) {
    const double speedup = time > 0 ? serial / time : 0;
    const double efficiency = speedup / threads;

    std::cout << std::left << std::setw(9) << threads
              << std::right << std::fixed << std::setprecision(6)
              << std::setw(16) << time
              << std::setprecision(3) << std::setw(13) << speedup
              << std::setw(15) << efficiency
              << std::setw(12) << (ok ? "PASS" : "FAIL") << '\n';
}

// Measure only the operation passed to this helper, using OpenMP wall time.
template <class F>
double timed(F&& operation) {
    const double start = omp_get_wtime();
    operation();
    return omp_get_wtime() - start;
}
