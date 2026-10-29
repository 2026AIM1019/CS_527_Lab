
// Includes the common header file, which provides commonly used libraries
// and helper functions for timing, thread counts, and printing results.
#include "../common.h"

// Includes random-number generation tools for creating test arrays.
#include <random>

using namespace std;

// An unnamed namespace makes the functions inside it accessible
// only within this source file.
namespace
{
    // Merges two sorted sections of the values array into one sorted section.
    //
    // begin  = starting index of the complete section
    // middle = starting index of the second sorted section
    // end    = one position past the last index of the complete section
    //
    // The two sorted sections are [begin, middle) and [middle, end).
    void merge_ranges(vector<int> &values, vector<int> &temporary, size_t begin,
                      size_t middle, size_t end)
    {
        // i points to the current element in the first sorted section.
        // j points to the current element in the second sorted section.
        // out points to the next position to fill in the temporary array.
        size_t i = begin, j = middle, out = begin;

        // Continue while both sorted sections still contain elements.
        while (i < middle && j < end)
        {
            // If the current element in the first section is smaller
            // or equal, copy it into the temporary array first.
            if (values[i] <= values[j])
                temporary[out++] = values[i++];

            // Otherwise, copy the current element from the second section.
            else
                temporary[out++] = values[j++];
        }

        // Copy any remaining elements from the first section.
        // This loop runs if the second section becomes empty first.
        while (i < middle)
            temporary[out++] = values[i++];

        // Copy any remaining elements from the second section.
        // This loop runs if the first section becomes empty first.
        while (j < end)
            temporary[out++] = values[j++];

        // Copy the merged, sorted section back into the original array.
        //
        // begin and end define the exact range being copied.
        // static_cast<ptrdiff_t> converts the indexes to the signed
        // iterator-difference type used for iterator arithmetic.
        copy(temporary.begin() + static_cast<ptrdiff_t>(begin),
             temporary.begin() + static_cast<ptrdiff_t>(end),
             values.begin() + static_cast<ptrdiff_t>(begin));
    }

    // Recursively sorts the section [begin, end) using merge sort.
    // The depth parameter controls how far the algorithm creates
    // parallel OpenMP tasks.
    void parallel_merge_sort_impl(vector<int> &values, vector<int> &temporary,
                                  size_t begin, size_t end, int depth)
    {
        // Sections containing at most 2048 elements are sorted directly
        // using the standard library's sort() function.
        // This avoids creating too many small recursive tasks.
        constexpr size_t cutoff = 2048;

        // Base case: if the section is small enough, sort it and return.
        if (end - begin <= cutoff)
        {
            // Sort only the selected section of the array.
            sort(values.begin() + static_cast<ptrdiff_t>(begin),
                 values.begin() + static_cast<ptrdiff_t>(end));
            return;
        }

        // Find the middle index to divide the section into two halves.
        // This formula avoids adding begin and end directly.
        const size_t middle = begin + (end - begin) / 2;

        // Create parallel tasks only while the recursion depth is below 4.
        // This limits task creation and helps reduce scheduling overhead.
        if (depth < 4)
        {
            // Create an OpenMP task to sort the first half.
            //
            // shared(values, temporary):
            //     The task uses the same arrays as the other tasks.
            //
            // firstprivate(begin, middle, depth):
            //     Each task receives its own copy of these index and
            //     depth values, so they do not interfere with other tasks.
#pragma omp task shared(values, temporary) firstprivate(begin, middle, depth)
            parallel_merge_sort_impl(values, temporary, begin, middle, depth + 1);

            // Create another OpenMP task to sort the second half.
            // It works on a different section of the values array.
#pragma omp task shared(values, temporary) firstprivate(middle, end, depth)
            parallel_merge_sort_impl(values, temporary, middle, end, depth + 1);

            // Wait until both child tasks have completed their sorting.
            // The two halves must be sorted before they can be merged.
#pragma omp taskwait
        }
        else
        {
            // Once the depth limit is reached, continue recursively
            // without creating additional OpenMP tasks.
            // These recursive calls still divide the array into halves,
            // but they execute as ordinary function calls.
            parallel_merge_sort_impl(values, temporary, begin, middle, depth + 1);
            parallel_merge_sort_impl(values, temporary, middle, end, depth + 1);
        }

        // Merge the two sorted halves into one sorted section.
        //
        // This operation must happen after both halves are sorted.
        // Each recursive call merges its own section of the array.
        merge_ranges(values, temporary, begin, middle, end);
    }

    // Starts the parallel merge-sort algorithm for the entire array.
    // The threads argument specifies the requested number of OpenMP threads.
    void parallel_merge_sort(vector<int> &values, int threads)
    {
        // Create a temporary array of the same size as the input array.
        // It is used to store elements while merging sorted sections.
        vector<int> temporary(values.size());

        // Start an OpenMP parallel region using the requested thread count.
        // All threads share the original array and the temporary array.
#pragma omp parallel num_threads(threads) shared(values, temporary)
        {
            // Only one thread starts the recursive sorting procedure.
            // The tasks created during recursion can then be executed
            // by available threads in the OpenMP team.
#pragma omp single
            parallel_merge_sort_impl(values, temporary, 0, values.size(), 0);
        }
        // The parallel region ends after its work has completed.
    }
}

// Runs the sorting experiment for different array sizes and thread counts.
void run_task2()
{
    // Print the task name and explain the parallel sorting approach.
    // The output also explains why synchronization is required before merging.
    cout << "\nTask 2: Parallel Array Sorting (task-parallel merge sort)\n"
         << "OpenMP tasks sort disjoint halves; taskwait is required before merging them.\n"
         << "The merge itself writes disjoint ranges, so no lock is needed.\n";

    // Define the three input sizes used for performance testing.
    const vector<size_t> sizes{100000, 500000, 1000000};

    // Create a random-number generator with a fixed seed.
    // Using the same seed makes the generated input repeatable across runs.
    mt19937 generator(527U);

    // Random array elements will be integers between -1000000 and 1000000.
    uniform_int_distribution<int> distribution(-1000000, 1000000);

    // Run the experiment separately for each array size.
    for (const size_t n : sizes)
    {
        // Create the original input array.
        vector<int> original(n);

        // Fill the array with random integers from the distribution.
        for (int &value : original)
            value = distribution(generator);

        // Create a reference copy of the input.
        vector<int> reference = original;

        // Sort the reference array using the standard serial sorting algorithm.
        // This result will be used to verify the parallel algorithm.
        sort(reference.begin(), reference.end());

        // Create a separate copy for measuring serial sorting time.
        vector<int> serial_work = original;

        // Perform a preliminary serial sort as a warm-up.
        // This is not included in the measured samples.
        sort(serial_work.begin(), serial_work.end()); // warm-up

        // Store the execution times from the serial sorting measurements.
        vector<double> serial_samples;

        // Run the serial sort three times to collect timing samples.
        for (int sample = 0; sample < 3; ++sample)
        {
            // Restore the original unsorted input before every measurement.
            // Otherwise, later samples would sort an already sorted array.
            serial_work = original;

            // Record the starting time.
            const double start = omp_get_wtime();

            // Sort the complete array using the standard serial sort().
            sort(serial_work.begin(), serial_work.end());

            // Save the elapsed time for this sample.
            serial_samples.push_back(omp_get_wtime() - start);
        }

        // Calculate the median of the three serial execution times.
        // The helper function is provided by common.h.
        const double serial_time = median_seconds(serial_samples);

        // Check that the reference result is sorted.
        // This provides a basic check of the serial reference array.
        const bool serial_sorted = is_sorted(reference.begin(), reference.end());

        // Display the array size and the median serial execution time.
        cout << "\nArray size: " << n << "\nSerial sort median time: "
             << fixed << setprecision(9) << serial_time << " s\n";

        // Print the column headings for the performance comparison table.
        print_timing_header();

        // Repeat the parallel experiment for each configured thread count.
        for (const int threads : thread_counts())
        {
            // Make a fresh copy of the original unsorted array.
            vector<int> result = original;

            // Restore the original input before the warm-up call.
            // This assignment is redundant because result was just initialized
            // from original, but it is retained to preserve the original code.
            result = original;

            // Run the parallel merge sort once as a warm-up.
            // Its execution time is not included in the measured samples.
            parallel_merge_sort(result, threads); // warm-up before the measured samples

            // Store the execution times of the parallel sorting runs.
            vector<double> samples;

            // Measure the parallel sort three times.
            for (int sample = 0; sample < 3; ++sample)
            {
                // Restore the original unsorted input before each run.
                result = original;

                // Record the starting time.
                const double start = omp_get_wtime();

                // Sort the array using the parallel merge-sort implementation.
                parallel_merge_sort(result, threads);

                // Save the elapsed time for this run.
                samples.push_back(omp_get_wtime() - start);
            }

            // Calculate the median parallel execution time.
            const double elapsed = median_seconds(samples);

            // Verify the parallel result in three ways:
            //
            // 1. serial_sorted:
            //    The reference result passes the basic sortedness check.
            //
            // 2. result == reference:
            //    Every element in the parallel output matches the serial result.
            //
            // 3. is_sorted(result.begin(), result.end()):
            //    The parallel output is in sorted order.
            //
            // PASS means all these conditions are true.
            const bool pass = serial_sorted && result == reference && is_sorted(result.begin(), result.end());

            // Display the thread count, parallel execution time,
            // speedup, efficiency, and correctness result.
            print_timing_row(threads, elapsed, serial_time, pass);
        }
    }

    // Explain how correctness is checked after parallel sorting.
    // The entire output is compared with the standard serial sort result.
    cout << "Verification compares the complete parallel output against sort's serial result.\n";
}

