#include <omp.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
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

struct MinMaxResult {
    int maxValue;
    long long maxIndex;
    int minValue;
    long long minIndex;
};

MinMaxResult serialMinMax(const vector<int>& a) {
    MinMaxResult r{a[0], 0, a[0], 0};
    for (long long i = 1; i < static_cast<long long>(a.size()); ++i) {
        const int value = a[static_cast<size_t>(i)];
        if (value > r.maxValue) { r.maxValue = value; r.maxIndex = i; }
        if (value < r.minValue) { r.minValue = value; r.minIndex = i; }
    }
    return r;
}

void parallelValues(const vector<int>& a, int& maxValue, int& minValue) {
    maxValue = numeric_limits<int>::min();
    minValue = numeric_limits<int>::max();
    const long long n = static_cast<long long>(a.size());
    #pragma omp parallel for reduction(max:maxValue) reduction(min:minValue)
    for (long long i = 0; i < n; ++i) {
        const int value = a[static_cast<size_t>(i)];
        if (value > maxValue) maxValue = value;
        if (value < minValue) minValue = value;
    }
}

void indicesCritical(const vector<int>& a, long long& maxIndex, long long& minIndex) {
    int bestMax = a[0], bestMin = a[0];
    maxIndex = 0;
    minIndex = 0;
    const long long n = static_cast<long long>(a.size());
    #pragma omp parallel for
    for (long long i = 1; i < n; ++i) {
        #pragma omp critical
        {
            const int value = a[static_cast<size_t>(i)];
            if (value > bestMax || (value == bestMax && i < maxIndex)) {
                bestMax = value;
                maxIndex = i;
            }
            if (value < bestMin || (value == bestMin && i < minIndex)) {
                bestMin = value;
                minIndex = i;
            }
        }
    }
}

void indicesManual(const vector<int>& a, long long& maxIndex, long long& minIndex) {
    const int capacity = omp_get_max_threads();
    vector<int> localMax(static_cast<size_t>(capacity), numeric_limits<int>::min());
    vector<int> localMin(static_cast<size_t>(capacity), numeric_limits<int>::max());
    vector<long long> localMaxIndex(static_cast<size_t>(capacity), numeric_limits<long long>::max());
    vector<long long> localMinIndex(static_cast<size_t>(capacity), numeric_limits<long long>::max());
    const long long n = static_cast<long long>(a.size());

    #pragma omp parallel
    {
        const int tid = omp_get_thread_num();
        #pragma omp for
        for (long long i = 0; i < n; ++i) {
            const int value = a[static_cast<size_t>(i)];
            if (value > localMax[tid] || (value == localMax[tid] && i < localMaxIndex[tid])) {
                localMax[tid] = value;
                localMaxIndex[tid] = i;
            }
            if (value < localMin[tid] || (value == localMin[tid] && i < localMinIndex[tid])) {
                localMin[tid] = value;
                localMinIndex[tid] = i;
            }
        }
    }

    maxIndex = numeric_limits<long long>::max();
    minIndex = numeric_limits<long long>::max();
    int bestMax = numeric_limits<int>::min();
    int bestMin = numeric_limits<int>::max();
    for (int t = 0; t < capacity; ++t) {
        if (localMax[t] > bestMax || (localMax[t] == bestMax && localMaxIndex[t] < maxIndex)) {
            bestMax = localMax[t];
            maxIndex = localMaxIndex[t];
        }
        if (localMin[t] < bestMin || (localMin[t] == bestMin && localMinIndex[t] < minIndex)) {
            bestMin = localMin[t];
            minIndex = localMinIndex[t];
        }
    }
}

bool validIndices(const vector<int>& a, const MinMaxResult& expected, long long maxIndex, long long minIndex) {
    return maxIndex >= 0 && minIndex >= 0 &&
           static_cast<size_t>(maxIndex) < a.size() && static_cast<size_t>(minIndex) < a.size() &&
           a[static_cast<size_t>(maxIndex)] == expected.maxValue &&
           a[static_cast<size_t>(minIndex)] == expected.minValue &&
           maxIndex == expected.maxIndex && minIndex == expected.minIndex;
}

void runTask3() {
    const vector<long long> sizes = {10000000, 100000000};
    const vector<int> threadCounts = {1, 2, 4, 8};
    cout << "\nTask 3: Maximum and Minimum of an Array\n";
    cout << "Timings are medians of 3 runs after a warm-up; ties select the smallest index.\n";

    for (long long n : sizes) {
        cout << "\nN = " << n << "\n";
        vector<int> a;
        try {
            a.resize(static_cast<size_t>(n));
        } catch (const bad_alloc&) {
            cout << "Skipped N = " << n << ": not enough memory.\n";
            continue;
        }
        for (long long i = 0; i < n; ++i)
            a[static_cast<size_t>(i)] = static_cast<int>((i * 37 + 13) % 1000003) - 500001;

        const MinMaxResult expected = serialMinMax(a);
        const double serialTime = medianTime([&]() { volatile MinMaxResult r = serialMinMax(a); (void)r; });
        cout << "Serial max = " << expected.maxValue << " at index " << expected.maxIndex
             << "; min = " << expected.minValue << " at index " << expected.minIndex
             << "; median time = " << fixed << setprecision(9) << serialTime << " s\n";

        cout << left << setw(9) << "Threads"
             << right << setw(18) << "Values (s)"
             << setw(20) << "Critical idx (s)"
             << setw(19) << "Manual idx (s)"
             << setw(12) << "Check" << '\n';
        cout << string(78, '-') << '\n';

        for (int threads : threadCounts) {
            omp_set_num_threads(threads);
            int maxValue = 0, minValue = 0;
            const double valueTime = medianTime([&]() { parallelValues(a, maxValue, minValue); });
            parallelValues(a, maxValue, minValue);
            const bool valuesOk = maxValue == expected.maxValue && minValue == expected.minValue;

            long long criticalMax = 0, criticalMin = 0;
            // The per-element critical section is intentionally expensive, but use 3 samples for a median.
            const double criticalTime = medianTime([&]() { indicesCritical(a, criticalMax, criticalMin); }, 3);
            indicesCritical(a, criticalMax, criticalMin);
            const bool criticalOk = validIndices(a, expected, criticalMax, criticalMin);

            long long manualMax = 0, manualMin = 0;
            const double manualTime = medianTime([&]() { indicesManual(a, manualMax, manualMin); });
            indicesManual(a, manualMax, manualMin);
            const bool manualOk = validIndices(a, expected, manualMax, manualMin);

            cout << left << setw(9) << threads
                 << right << setw(18) << fixed << setprecision(9) << valueTime
                 << setw(20) << criticalTime
                 << setw(19) << manualTime
                 << setw(12) << ((valuesOk && criticalOk && manualOk) ? "PASS" : "FAIL") << '\n';
        }
    }
    cout << "\nChecks verify the extrema values and exact smallest-index tie-breaking rule.\n";
}
