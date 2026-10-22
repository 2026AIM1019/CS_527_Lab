#include "../common.h"
#include <array>
#include <random>

using namespace std;

namespace
{
    volatile long long histogram_sink = 0;
    using Histogram = array<long long, 10>;
    Histogram serial_histogram(const vector<int> &values)
    {
        Histogram counts{};
        for (const int value : values)
            ++counts[static_cast<size_t>(value)];
        return counts;
    }
    Histogram parallel_histogram(const vector<int> &values, int threads)
    {
        vector<Histogram> local(static_cast<size_t>(threads));
        for (auto &counts : local)
            counts.fill(0);
#pragma omp parallel num_threads(threads) shared(local, values)
        {
            const int id = omp_get_thread_num();
            Histogram &counts = local[static_cast<size_t>(id)];
#pragma omp for schedule(static)
            for (long long i = 0; i < static_cast<long long>(values.size()); ++i)
            {
                ++counts[static_cast<size_t>(values[static_cast<size_t>(i)])];
            }
        }
        Histogram total{};
        for (const auto &counts : local)
        {
            for (size_t bin = 0; bin < total.size(); ++bin)
                total[bin] += counts[bin];
        }
        return total;
    }
}

void run_task3()
{
    cout << "\nTask 3: Histogram Calculation (values 0 through 9)\n"
         << "Each thread updates its own 10-bin histogram; private histograms are combined after the parallel loop.\n";
    const vector<size_t> sizes{1000000, 10000000, 50000000};
    mt19937 generator(3527U);
    uniform_int_distribution<int> distribution(0, 9);
    for (const size_t n : sizes)
    {
        vector<int> values(n);
        for (int &value : values)
            value = distribution(generator);
        const Histogram reference = serial_histogram(values);
        const double serial_time = measure_median([&]
                                                  {
            const Histogram counts = serial_histogram(values);
            histogram_sink = accumulate(counts.begin(), counts.end(), 0LL); });
        cout << "\nArray size: " << n << "\nSerial histogram median time: "
             << fixed << setprecision(9) << serial_time << " s\n";
        print_timing_header();
        for (const int threads : thread_counts())
        {
            Histogram result{};
            const double elapsed = measure_median([&]
                                                  { result = parallel_histogram(values, threads); });
            const bool pass = result == reference && accumulate(result.begin(), result.end(), 0LL) == static_cast<long long>(n);
            print_timing_row(threads, elapsed, serial_time, pass);
        }
        cout << "Frequencies: ";
        for (size_t bin = 0; bin < reference.size(); ++bin)
        {
            cout << bin << '=' << reference[bin] << (bin + 1 == reference.size() ? '\n' : ' ');
        }
    }
}
