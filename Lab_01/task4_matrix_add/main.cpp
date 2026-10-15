#include <omp.h>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <class Function>
double medianTime(Function function, int samples = 3, int repetitions = 1) {
    function(); // Warm-up
    vector<double> times;
    times.reserve(samples);
    for (int sample = 0; sample < samples; ++sample) {
        const double start = omp_get_wtime();
        for (int rep = 0; rep < repetitions; ++rep) function();
        times.push_back((omp_get_wtime() - start) / repetitions);
    }
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

void addSerial(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            const size_t index = static_cast<size_t>(i) * n + j;
            c[index] = a[index] + b[index];
        }
}

void addOuter(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            const size_t index = static_cast<size_t>(i) * n + j;
            c[index] = a[index] + b[index];
        }
}

void addCollapse(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            const size_t index = static_cast<size_t>(i) * n + j;
            c[index] = a[index] + b[index];
        }
}

bool sameTask4(const vector<double>& a, const vector<double>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (fabs(a[i] - b[i]) > 1e-12) return false;
    return true;
}

void runTask4() {
    const vector<int> sizes = {8, 4096};
    const vector<int> threadCounts = {1, 2, 4, 8};
    cout << "\nTask 4: Matrix Addition (flattened row-major matrices)\n";
    cout << "Timings are median per-call times from 3 samples after a warm-up.\n";
    cout << "For N=8, each sample batches 200 calls to reduce timer-resolution noise.\n";

    for (int n : sizes) {
        const size_t total = static_cast<size_t>(n) * n;
        cout << "\nMatrix size: " << n << " x " << n << "\n";
        vector<double> a(total), b(total), serial(total), result(total);
        for (size_t i = 0; i < total; ++i) {
            a[i] = static_cast<double>(i % 101) * 0.5;
            b[i] = static_cast<double>(i % 79) * 0.25;
        }

        const int repetitions = n == 8 ? 200 : 1;
        const double serialTime = medianTime([&]() { addSerial(a, b, serial, n); }, 3, repetitions);
        cout << "Serial median time: " << fixed << setprecision(9) << serialTime << " s\n";

        cout << left << setw(9) << "Threads"
             << right << setw(18) << "Outer (s)"
             << setw(16) << "Outer check"
             << setw(18) << "Collapse (s)"
             << setw(18) << "Collapse check" << '\n';
        cout << string(79, '-') << '\n';

        for (int threads : threadCounts) {
            omp_set_num_threads(threads);
            const double outerTime = medianTime([&]() { addOuter(a, b, result, n); }, 3, repetitions);
            const bool outerOk = sameTask4(serial, result);
            const double collapseTime = medianTime([&]() { addCollapse(a, b, result, n); }, 3, repetitions);
            const bool collapseOk = sameTask4(serial, result);

            cout << left << setw(9) << threads
                 << right << setw(18) << fixed << setprecision(9) << outerTime
                 << setw(16) << (outerOk ? "PASS" : "FAIL")
                 << setw(18) << collapseTime
                 << setw(18) << (collapseOk ? "PASS" : "FAIL") << '\n';
        }
    }
}
