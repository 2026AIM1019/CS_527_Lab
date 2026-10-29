
// Includes common libraries and helper functions from the parent directory.
// These include timing functions, thread-count utilities, and output helpers.
#include "../common.h"

// Provides std::array, which stores a fixed number of elements.
#include <array>

// Provides random-number generation facilities.
#include <random>

using namespace std;

// An unnamed namespace restricts the variables, type aliases, and functions
// declared inside it to this source file.
namespace
{
    // Stores a value calculated during timing so that the compiler
    // cannot simply discard the histogram calculation as unused.
    // 'volatile' tells the compiler that this variable may change unexpectedly.
    volatile long long histogram_sink = 0;

    // Defines a histogram containing exactly 10 bins.
    // Each bin uses long long to store the frequency of a value.
    // Bin 0 counts zeros, bin 1 counts ones, and so on up to bin 9.
    using Histogram = array<long long, 10>;

    // Calculates the histogram sequentially using a single thread.
    // It returns the frequency of each value from 0 through 9.
    Histogram serial_histogram(const vector<int> &values)
    {
        // Create a histogram and initialize every bin to zero.
        Histogram counts{};

        // Visit each value in the input vector.
        for (const int value : values)

            // Increase the bin corresponding to the current value.
            // For example, if value == 3, increment counts[3].
            // The cast converts the integer value to the vector-style
            // unsigned index type used for array indexing.
            ++counts[static_cast<size_t>(value)];

        // Return the completed histogram.
        return counts;
    }

    // Calculates the histogram in parallel using OpenMP.
    // Each thread builds its own histogram to avoid concurrent updates
    // to the same bin.
    Histogram parallel_histogram(const vector<int> &values, int threads)
    {
        // Create one histogram for each requested thread.
        // Each histogram has 10 bins.
        vector<Histogram> local(static_cast<size_t>(threads));

        // Initialize all bins in every thread's histogram to zero.
        for (auto &counts : local)
            counts.fill(0);

        // Start an OpenMP parallel region using the requested number
        // of threads. All threads can read values and access local.
#pragma omp parallel num_threads(threads) shared(local, values)
        {
            // Get the unique thread ID of the current thread.
            // Thread IDs normally range from 0 to threads - 1.
            const int id = omp_get_thread_num();

            // Select the histogram belonging to this thread.
            // Different threads update different Histogram objects.
            Histogram &counts = local[static_cast<size_t>(id)];

            // Divide the loop iterations among the threads.
            // schedule(static) assigns chunks of iterations to threads
            // in a predetermined way.
#pragma omp for schedule(static)
            // Visit every element in the input vector.
            // long long is used for the loop counter.
            for (long long i = 0; i < static_cast<long long>(values.size()); ++i)
            {
                // Read the current value and increment its bin in
                // this thread's private histogram.
                //
                // For example, if the value is 7, increment counts[7].
                // Because each thread updates its own histogram,
                // no lock or atomic operation is needed for this increment.
                ++counts[static_cast<size_t>(values[static_cast<size_t>(i)])];
            }
        }

        // The parallel region has finished, so all threads have completed
        // their updates to their individual histograms.

        // Create the final histogram, with all bins initially zero.
        Histogram total{};

        // Visit each thread's completed histogram.
        for (const auto &counts : local)
        {
            // Add each bin's count to the corresponding bin
            // in the final histogram.
            for (size_t bin = 0; bin < total.size(); ++bin)
                total[bin] += counts[bin];
        }

        // Return the combined histogram containing the final frequencies.
        return total;
    }
}

// Runs the histogram experiment for different input sizes and thread counts.
void run_task3()
{
    // Display the task name and explain the parallel approach.
    // Each thread counts values in its own histogram, and those histograms
    // are combined after the parallel loop has finished.
    cout << "\nTask 3: Histogram Calculation (values 0 through 9)\n"
         << "Each thread updates its own 10-bin histogram; private histograms are combined after the parallel loop.\n";

    // Define the array sizes used in the experiment.
    // Larger arrays help show how the parallel version behaves as
    // the amount of work increases.
    const vector<size_t> sizes{1000000, 10000000, 50000000};

    // Create a random-number generator with a fixed seed.
    // Using the same seed makes the sequence of generated values repeatable.
    mt19937 generator(3527U);

    // Generate integers from 0 through 9, inclusive.
    // These values correspond directly to the 10 histogram bins.
    uniform_int_distribution<int> distribution(0, 9);

    // Run the experiment separately for each array size.
    for (const size_t n : sizes)
    {
        // Create a vector containing n integers.
        vector<int> values(n);

        // Fill the vector with randomly generated values.
        // Each value will be between 0 and 9.
        for (int &value : values)
            value = distribution(generator);

        // Calculate the correct histogram using the serial algorithm.
        // This histogram is used as the reference for checking
        // the parallel results.
        const Histogram reference = serial_histogram(values);

        // Measure the median execution time of the serial histogram.
        //
        // measure_median() is provided by common.h.
        // The lambda calculates the histogram and stores the sum of
        // its bins in histogram_sink to keep the result observable.
        const double serial_time = measure_median([&]
                                                  {
            // Calculate the serial histogram.
            const Histogram counts = serial_histogram(values);

            // Add all bin counts together.
            // Since every input value belongs to exactly one bin,
            // the total should equal the number of input elements.
            histogram_sink = accumulate(counts.begin(), counts.end(), 0LL); });

        // Display the array size and the measured serial execution time.
        cout << "\nArray size: " << n << "\nSerial histogram median time: "
             << fixed << setprecision(9) << serial_time << " s\n";

        // Print the headings for the timing results.
        // The helper function is provided by common.h.
        print_timing_header();

        // Run the parallel histogram with each thread count
        // returned by thread_counts().
        for (const int threads : thread_counts())
        {
            // Create a histogram to store the parallel result.
            // All bins are initialized to zero.
            Histogram result{};

            // Measure the median execution time of the parallel algorithm.
            // The result variable receives the histogram returned by
            // parallel_histogram().
            const double elapsed = measure_median([&]
                                                  { result = parallel_histogram(values, threads); });

            // Check that the parallel histogram matches the serial reference.
            // Also check that the sum of all bins equals the array size.
            //
            // The first condition checks every bin against the reference.
            // The second condition checks that all n elements were counted.
            const bool pass = result == reference && accumulate(result.begin(), result.end(), 0LL) == static_cast<long long>(n);

            // Print the thread count, elapsed time, speedup, efficiency,
            // and whether the correctness check passed.
            print_timing_row(threads, elapsed, serial_time, pass);
        }

        // Print the frequency of every possible value, from 0 through 9.
        cout << "Frequencies: ";

        // Visit every bin in the reference histogram.
        for (size_t bin = 0; bin < reference.size(); ++bin)
        {
            // Print the bin number and its frequency.
            // Print a newline after the last bin; otherwise, print a space.
            cout << bin << '=' << reference[bin] << (bin + 1 == reference.size() ? '\n' : ' ');
        }
    }
}

