
// Includes the common header file, which contains commonly used libraries
// and helper functions such as timing and thread-count utilities.
#include "../common.h"

// Includes limits and constants for integer types.
#include <climits>

using namespace std;

// An unnamed namespace makes the variables and functions inside it
// accessible only within this source file.
namespace
{
    // A variable used to store a result from the search so that the
    // compiler cannot simply treat the search operation as unnecessary.
    // 'volatile' tells the compiler that this variable may change unexpectedly.
    volatile long long search_sink = 0;

    // Performs a normal sequential search through the array.
    // It returns the index of the first occurrence of the target.
    // If the target is not present, it returns the array size.
    long long serial_search(const vector<int> &values, int target)
    {
        // find() searches from the beginning to the end of the vector
        // and returns an iterator pointing to the first matching element.
        const auto it = find(values.begin(), values.end(), target);

        // If the iterator reaches the end, the target was not found,
        // so return the size of the vector.
        // Otherwise, subtract the beginning iterator to get the index
        // of the first matching element.
        return it == values.end() ? static_cast<long long>(values.size())
                                  : static_cast<long long>(it - values.begin());
    }

    // Searches for the target using multiple threads with OpenMP.
    // The 'threads' argument specifies the number of threads to use.
    // Like serial_search(), it returns the smallest matching index,
    // or the array size if the target is not found.
    long long parallel_search(const vector<int> &values, int target, int threads)
    {
        // Initially assume that the target is not found.
        // The array size is used as the not-found value.
        long long first_index = static_cast<long long>(values.size());

        // Store the array size in a long long variable so that it can
        // be used as the upper bound of the loop.
        const long long n = static_cast<long long>(values.size());

        // OpenMP parallelizes the loop across the specified number of threads.
        //
        // num_threads(threads):
        //     Requests the specified number of threads.
        //
        // reduction(min : first_index):
        //     Gives each thread a private copy of first_index.
        //     At the end, OpenMP combines the copies using the minimum value.
        //     This ensures that the smallest matching index is returned.
        //
        // schedule(static):
        //     Divides the loop iterations into relatively fixed chunks
        //     assigned to threads before the loop executes.
#pragma omp parallel for num_threads(threads) reduction(min : first_index) schedule(static)
        for (long long i = 0; i < n; ++i)
        {
            // Convert the loop index to size_t because vector indexing
            // normally uses an unsigned size type.
            // If the current element matches the target and its index
            // is smaller than the current local result, update the result.
            if (values[static_cast<size_t>(i)] == target && i < first_index)
            {
                first_index = i;
            }
        }

        // After the parallel loop finishes, the reduction has combined
        // the thread-local results. Return the smallest matching index,
        // or n if no thread found the target.
        return first_index;
    }
}

// Runs the array-search experiment and displays its timing results.
void run_task1()
{
    // Display the task name and explain how the parallel search works.
    // The array size n is used as the not-found result.
    cout << "\nTask 1: Array Search\n"
         << "OpenMP reduction(min:...) returns the first matching index; n means not found.\n";

    // These are the three array sizes used in the experiment.
    // The program tests small, medium, and large arrays to compare performance.
    const vector<size_t> sizes{1000000, 5000000, 10000000};

    // Run the experiment separately for every array size.
    for (const size_t n : sizes)
    {
        // Create a vector containing n integer elements.
        vector<int> values(n);

        // Fill the array with deterministic values.
        // The formula generates values in the range 0 to 100002.
        // Using a formula makes the test data repeatable.
        for (size_t i = 0; i < n; ++i)
        {
            values[i] = static_cast<int>((i * 37U + i / 7U) % 100003U);
        }

        // Choose a target value that is outside the range generated
        // by the formula, so it will not match any generated element.
        const int target = -777777;

        // Place the target at the last position in the array.
        // This makes the target present and requires the sequential
        // search to examine almost the entire array before finding it.
        values[n - 1] = target;

        // Run the serial search once and save its result.
        // This index is used as the correct answer for comparison.
        const long long serial_index = serial_search(values, target);

        // Measure the median execution time of the serial search.
        //
        // measure_median() is provided by common.h.
        // The lambda function runs the search and stores its result
        // in search_sink so that the computation is not discarded.
        const double serial_time = measure_median([&]
                                                  { search_sink = serial_search(values, target); });

        // Print the array size, target, search result, index, and timing.
        // If the returned index equals n, display NOT FOUND and index -1.
        // Otherwise, display FOUND and the actual zero-based index.
        cout << "\nArray size: " << n << ", target: " << target
             << "\nSerial result: " << (serial_index == static_cast<long long>(n) ? "NOT FOUND" : "FOUND")
             << ", index = " << (serial_index == static_cast<long long>(n) ? -1 : serial_index)
             << ", median time = " << fixed << setprecision(9) << serial_time << " s\n";

        // Print the table heading for the parallel timing results.
        // The helper function is defined in common.h.
        print_timing_header();

        // Test the parallel search using each thread count returned
        // by thread_counts(), which is provided by common.h.
        for (const int threads : thread_counts())
        {
            // Initially set the result to n, which means not found.
            // The parallel search will replace it with the smallest
            // matching index if the target exists.
            long long parallel_index = static_cast<long long>(n);

            // Measure the median execution time of the parallel search.
            // The lambda runs the search with the current thread count
            // and stores the returned index in parallel_index.
            const double elapsed = measure_median([&]
                                                  { parallel_index = parallel_search(values, target, threads); });

            // Compare the parallel result with the serial result.
            // PASS means both methods returned the same index.
            const bool pass = parallel_index == serial_index;

            // Display the thread count, parallel timing, speedup,
            // efficiency, and correctness result.
            // The exact output format is handled by common.h.
            print_timing_row(threads, elapsed, serial_time, pass);
        }

        // Choose another target that is not present in the array.
        // This tests the not-found case separately.
        const int absent_target = -888888;

        // Search for the absent target using the serial algorithm.
        // The expected result is n because no element matches.
        const long long absent_serial = serial_search(values, absent_target);

        // Search for the same absent target using four OpenMP threads.
        const long long absent_parallel = parallel_search(values, absent_target, 4);

        // Print PASS if the serial search returns n and the parallel
        // search returns exactly the same result.
        cout << "Absent-target check (4 threads): "
             << ((absent_serial == static_cast<long long>(n) && absent_parallel == absent_serial) ? "PASS" : "FAIL")
             << "\n";
    }

    // Explain the index convention used by both algorithms:
    // indexes start at zero, and if a target occurs multiple times,
    // the smallest index is returned.
    cout << "Index convention: zero-based; if a value appears more than once, the smallest index is returned.\n";
}

