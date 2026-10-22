#include "../common.h"
#include <random>

using namespace std;

namespace
{
    void merge_ranges(vector<int> &values, vector<int> &temporary, size_t begin,
                      size_t middle, size_t end)
    {
        size_t i = begin, j = middle, out = begin;
        while (i < middle && j < end)
        {
            if (values[i] <= values[j])
                temporary[out++] = values[i++];
            else
                temporary[out++] = values[j++];
        }
        while (i < middle)
            temporary[out++] = values[i++];
        while (j < end)
            temporary[out++] = values[j++];
        copy(temporary.begin() + static_cast<ptrdiff_t>(begin),
             temporary.begin() + static_cast<ptrdiff_t>(end),
             values.begin() + static_cast<ptrdiff_t>(begin));
    }

    void parallel_merge_sort_impl(vector<int> &values, vector<int> &temporary,
                                  size_t begin, size_t end, int depth)
    {
        constexpr size_t cutoff = 2048;
        if (end - begin <= cutoff)
        {
            sort(values.begin() + static_cast<ptrdiff_t>(begin),
                 values.begin() + static_cast<ptrdiff_t>(end));
            return;
        }
        const size_t middle = begin + (end - begin) / 2;
        if (depth < 4)
        {
#pragma omp task shared(values, temporary) firstprivate(begin, middle, depth)
            parallel_merge_sort_impl(values, temporary, begin, middle, depth + 1);
#pragma omp task shared(values, temporary) firstprivate(middle, end, depth)
            parallel_merge_sort_impl(values, temporary, middle, end, depth + 1);
#pragma omp taskwait
        }
        else
        {
            parallel_merge_sort_impl(values, temporary, begin, middle, depth + 1);
            parallel_merge_sort_impl(values, temporary, middle, end, depth + 1);
        }
        merge_ranges(values, temporary, begin, middle, end);
    }

    void parallel_merge_sort(vector<int> &values, int threads)
    {
        vector<int> temporary(values.size());
#pragma omp parallel num_threads(threads) shared(values, temporary)
        {
#pragma omp single
            parallel_merge_sort_impl(values, temporary, 0, values.size(), 0);
        }
    }
}

void run_task2()
{
    cout << "\nTask 2: Parallel Array Sorting (task-parallel merge sort)\n"
         << "OpenMP tasks sort disjoint halves; taskwait is required before merging them.\n"
         << "The merge itself writes disjoint ranges, so no lock is needed.\n";
    const vector<size_t> sizes{100000, 500000, 1000000};
    mt19937 generator(527U);
    uniform_int_distribution<int> distribution(-1000000, 1000000);
    for (const size_t n : sizes)
    {
        vector<int> original(n);
        for (int &value : original)
            value = distribution(generator);
        vector<int> reference = original;
        sort(reference.begin(), reference.end());
        vector<int> serial_work = original;
        sort(serial_work.begin(), serial_work.end()); // warm-up
        vector<double> serial_samples;
        for (int sample = 0; sample < 3; ++sample)
        {
            serial_work = original;
            const double start = omp_get_wtime();
            sort(serial_work.begin(), serial_work.end());
            serial_samples.push_back(omp_get_wtime() - start);
        }
        const double serial_time = median_seconds(serial_samples);
        const bool serial_sorted = is_sorted(reference.begin(), reference.end());
        cout << "\nArray size: " << n << "\nSerial sort median time: "
             << fixed << setprecision(9) << serial_time << " s\n";
        print_timing_header();
        for (const int threads : thread_counts())
        {
            vector<int> result = original;
            result = original;
            parallel_merge_sort(result, threads); // warm-up before the measured samples
            vector<double> samples;
            for (int sample = 0; sample < 3; ++sample)
            {
                result = original;
                const double start = omp_get_wtime();
                parallel_merge_sort(result, threads);
                samples.push_back(omp_get_wtime() - start);
            }
            const double elapsed = median_seconds(samples);
            const bool pass = serial_sorted && result == reference && is_sorted(result.begin(), result.end());
            print_timing_row(threads, elapsed, serial_time, pass);
        }
    }
    cout << "Verification compares the complete parallel output against sort's serial result.\n";
}
