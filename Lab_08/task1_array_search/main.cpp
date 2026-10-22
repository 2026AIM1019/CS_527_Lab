#include "../common.h"
#include <climits>

using namespace std;

namespace
{
    volatile long long search_sink = 0;
    long long serial_search(const vector<int> &values, int target)
    {
        const auto it = find(values.begin(), values.end(), target);

        return it == values.end() ? static_cast<long long>(values.size())
                                  : static_cast<long long>(it - values.begin());
    }

    long long parallel_search(const vector<int> &values, int target, int threads)
    {
        long long first_index = static_cast<long long>(values.size());
        const long long n = static_cast<long long>(values.size());

#pragma omp parallel for num_threads(threads) reduction(min : first_index) schedule(static)
        for (long long i = 0; i < n; ++i)
        {
            if (values[static_cast<size_t>(i)] == target && i < first_index)
            {
                first_index = i;
            }
        }
        return first_index;
    }
}

void run_task1()
{
    cout << "\nTask 1: Array Search\n"
         << "OpenMP reduction(min:...) returns the first matching index; n means not found.\n";
    const vector<size_t> sizes{1000000, 5000000, 10000000};

    for (const size_t n : sizes)
    {
        vector<int> values(n);
        for (size_t i = 0; i < n; ++i)
        {
            values[i] = static_cast<int>((i * 37U + i / 7U) % 100003U);
        }

        const int target = -777777;
        values[n - 1] = target;

        const long long serial_index = serial_search(values, target);
        const double serial_time = measure_median([&]
                                                  { search_sink = serial_search(values, target); });
        cout << "\nArray size: " << n << ", target: " << target
             << "\nSerial result: " << (serial_index == static_cast<long long>(n) ? "NOT FOUND" : "FOUND")
             << ", index = " << (serial_index == static_cast<long long>(n) ? -1 : serial_index)
             << ", median time = " << fixed << setprecision(9) << serial_time << " s\n";
        print_timing_header();
        for (const int threads : thread_counts())
        {
            long long parallel_index = static_cast<long long>(n);
            const double elapsed = measure_median([&]
                                                  { parallel_index = parallel_search(values, target, threads); });
            const bool pass = parallel_index == serial_index;
            print_timing_row(threads, elapsed, serial_time, pass);
        }
        const int absent_target = -888888;
        const long long absent_serial = serial_search(values, absent_target);
        const long long absent_parallel = parallel_search(values, absent_target, 4);
        cout << "Absent-target check (4 threads): "
             << ((absent_serial == static_cast<long long>(n) && absent_parallel == absent_serial) ? "PASS" : "FAIL")
             << "\n";
    }
    cout << "Index convention: zero-based; if a value appears more than once, the smallest index is returned.\n";
}
