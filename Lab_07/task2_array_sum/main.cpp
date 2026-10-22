#include <omp.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

template <class Function>
double medianTime(Function function, int samples = 3) {
    function();
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

long long sumSerial(const vector<long long>& a) {
    long long sum = 0;
    for (long long value : a) sum += value;
    return sum;
}

long long sumNaive(const vector<long long>& a) {
    long long sum = 0;
    const long long n = static_cast<long long>(a.size());
    #pragma omp parallel for
    for (long long i = 0; i < n; ++i) {
        sum += a[static_cast<size_t>(i)]; // Intentionally unsafe for the lab's data-race experiment.
    }
    return sum;
}

long long sumReduction(const vector<long long>& a) {
    long long sum = 0;
    const long long n = static_cast<long long>(a.size());
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < n; ++i) sum += a[static_cast<size_t>(i)];
    return sum;
}

void runTask2() {
    const vector<long long> sizes = {10000000, 100000000};
    const vector<int> threadCounts = {1, 2, 4, 8};
    cout << "\nTask 2: Sum of Array Elements\n";
    cout << "Naive sum is intentionally data-racy; its results are observations only, never valid answers.\n";
    cout << "Reduction timings are medians of 3 runs after a warm-up.\n";

    for (long long n : sizes) {
        cout << "\nN = " << n << "\n";
        vector<long long> a;
        try {
            a.resize(static_cast<size_t>(n));
        } catch (const bad_alloc&) {
            cout << "Skipped N = " << n << ": not enough memory.\n";
            continue;
        }
        for (long long i = 0; i < n; ++i) a[static_cast<size_t>(i)] = (i % 11) + 1;

        const double serialTime = medianTime([&]() { volatile long long result = sumSerial(a); (void)result; });
        const long long expected = sumSerial(a);
        cout << "Serial sum: " << expected << ", median time: " << fixed << setprecision(9) << serialTime << " s\n";

        cout << left << setw(9) << "Threads"
             << right << setw(16) << "Naive run 1"
             << setw(16) << "Naive run 2"
             << setw(16) << "Naive run 3"
             << setw(18) << "Naive median(s)"
             << setw(19) << "Reduction sum"
             << setw(18) << "Reduction(s)"
             << setw(12) << "Check" << '\n';
        cout << string(124, '-') << '\n';

        for (int threads : threadCounts) {
            omp_set_num_threads(threads);
            vector<long long> naiveResults(3);
            vector<double> naiveTimes(3);
            for (int run = 0; run < 3; ++run) {
                const double start = omp_get_wtime();
                naiveResults[run] = sumNaive(a);
                naiveTimes[run] = omp_get_wtime() - start;
            }
            vector<double> sortedTimes = naiveTimes;
            sort(sortedTimes.begin(), sortedTimes.end());
            const double naiveMedian = sortedTimes[1];

            const long long reductionResult = sumReduction(a); // Warm-up
            (void)reductionResult;
            const double reductionTime = medianTime([&]() { volatile long long result = sumReduction(a); (void)result; });
            const long long checkedResult = sumReduction(a);
            const bool ok = checkedResult == expected;

            cout << left << setw(9) << threads
                 << right << setw(16) << naiveResults[0]
                 << setw(16) << naiveResults[1]
                 << setw(16) << naiveResults[2]
                 << setw(18) << fixed << setprecision(9) << naiveMedian
                 << setw(19) << checkedResult
                 << setw(18) << reductionTime
                 << setw(12) << (ok ? "PASS" : "FAIL") << '\n';
        }
    }

    cout << "\nA critical section around every sum update can protect correctness, but serializes updates and is usually slower than reduction.\n";
}
