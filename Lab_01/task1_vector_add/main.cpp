#include <omp.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <new>
#include <stdexcept>
#include <vector>

using namespace std;

template <class Function>
double medianTime(Function function, int samples = 3) {
    function(); // Warm-up
    vector<double> times;
    times.reserve(samples);
    for (int sample = 0; sample < samples; ++sample) {
        double start = omp_get_wtime();
        function();
        times.push_back(omp_get_wtime() - start);
    }
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

void addSerial(const vector<double>& a, const vector<double>& b, vector<double>& c) {
    for (size_t i = 0; i < a.size(); ++i) c[i] = a[i] + b[i];
}

void addParallel(const vector<double>& a, const vector<double>& b, vector<double>& c) {
    const long long n = static_cast<long long>(a.size());
    #pragma omp parallel for
    for (long long i = 0; i < n; ++i) c[static_cast<size_t>(i)] = a[static_cast<size_t>(i)] + b[static_cast<size_t>(i)];
}

bool verifyAdd(const vector<double>& a, const vector<double>& b, const vector<double>& c) {
    if (a.size() != b.size() || a.size() != c.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (c[i] != a[i] + b[i]) return false;
    }
    return true;
}

void runTask1() {
    const vector<long long> sizes = {100000, 1000000, 10000000, 100000000};
    const vector<int> threadCounts = {1, 2, 4, 8};
    cout << "\nTask 1: Vector Addition\n";
    cout << "Times use omp_get_wtime(); each timing is the median of 3 runs after a warm-up.\n";

    for (long long n : sizes) {
        cout << "\nN = " << n << "\n";
        vector<double> a, b, c;
        try {
            a.resize(static_cast<size_t>(n));
            b.resize(static_cast<size_t>(n));
            c.resize(static_cast<size_t>(n));
        } catch (const bad_alloc&) {
            cout << "Skipped N = " << n << ": not enough memory to allocate arrays.\n";
            continue;
        } catch (const length_error&) {
            cout << "Skipped N = " << n << ": requested array size is too large.\n";
            continue;
        }

        for (long long i = 0; i < n; ++i) {
            a[static_cast<size_t>(i)] = static_cast<double>(i % 1000) * 0.5;
            b[static_cast<size_t>(i)] = static_cast<double>(i % 997) * 0.25;
        }

        const double serialTime = medianTime([&]() { addSerial(a, b, c); });
        const bool serialOk = verifyAdd(a, b, c);
        cout << "Serial median time: " << serialTime << " s, check: " << (serialOk ? "PASS" : "FAIL") << "\n";

        cout << left << setw(10) << "Threads"
             << right << setw(18) << "Parallel (s)"
             << setw(14) << "Check"
             << setw(16) << "Speedup" << '\n';
        cout << string(58, '-') << '\n';

        for (int threads : threadCounts) {
            omp_set_num_threads(threads);
            const double parallelTime = medianTime([&]() { addParallel(a, b, c); });
            const bool ok = verifyAdd(a, b, c);
            const double speedup = parallelTime > 0.0 ? serialTime / parallelTime : 0.0;
            cout << left << setw(10) << threads
                 << right << setw(18) << fixed << setprecision(9) << parallelTime
                 << setw(14) << (ok && serialOk ? "PASS" : "FAIL")
                 << setw(16) << setprecision(3) << speedup << '\n';
        }
    }
}
