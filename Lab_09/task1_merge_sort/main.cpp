#include "../common.h"
#include <random>  // Provides random number generation for test arrays.

namespace {
// Do not create new OpenMP tasks for ranges containing 4096 elements or fewer.
// Sorting small ranges serially helps avoid excessive task-management overhead.
constexpr int cutoff = 4096;

// Merge two already-sorted, adjacent ranges: [left, mid] and [mid + 1, right].
// a is the array being sorted, and tmp is a temporary array used during merging.
// Only the elements between left and right are modified.
void merge(int* a, int* tmp, int left, int mid, int right) {
    // i points to the current element in the left half.
    int i = left;

    // j points to the current element in the right half.
    int j = mid + 1;

    // k points to the next position to fill in the temporary array.
    int k = left;

    // Compare the current elements of both halves.
    // Copy the smaller element into tmp and move its pointer forward.
    // If both elements are equal, take the left element first.
    while (i <= mid && j <= right) {
        tmp[k++] = (a[i] <= a[j] ? a[i++] : a[j++]);
    }

    // If the left half still has elements, copy them into tmp.
    // These remaining elements are already in sorted order.
    while (i <= mid) tmp[k++] = a[i++];

    // If the right half still has elements, copy them into tmp.
    // These remaining elements are also already in sorted order.
    while (j <= right) tmp[k++] = a[j++];

    // Copy the merged, sorted range back into the original array.
    // right + 1 is used because the end iterator is exclusive.
    std::copy(tmp + left, tmp + right + 1, a + left);
}

// Standard recursive merge sort.
// It divides the array range into two halves, sorts each half,
// and then merges the two sorted halves together.
void serial(int* a, int* tmp, int left, int right) {
    // Base case: a range with zero or one element is already sorted.
    if (left >= right) return;

    // Find the middle index without adding left and right directly.
    const int mid = left + (right - left) / 2;

    // Recursively sort the left half.
    serial(a, tmp, left, mid);

    // Recursively sort the right half.
    serial(a, tmp, mid + 1, right);

    // Merge both sorted halves into one sorted range.
    merge(a, tmp, left, mid, right);
}

// Recursive task-parallel merge sort.
// Larger ranges are divided into OpenMP tasks, allowing different threads
// to sort separate parts of the array simultaneously.
// The child tasks work on disjoint array ranges, so they can share a and tmp.
void parallel(int* a, int* tmp, int left, int right) {
    // Stop when the range contains zero or one element.
    if (left >= right) return;

    // If the range is small enough, sort it serially instead of creating tasks.
    if (right - left + 1 <= cutoff) {
        serial(a, tmp, left, right);
        return;
    }

    // Calculate the middle index to divide the current range into two halves.
    const int mid = left + (right - left) / 2;

    // Create a task to sort the left half.
    // a and tmp are shared arrays.
    // firstprivate gives this task its own copies of left and mid.
#pragma omp task shared(a, tmp) firstprivate(left, mid)
    parallel(a, tmp, left, mid);

    // Create another task to sort the right half.
    // This task works on the range from mid + 1 to right.
    // firstprivate gives this task its own copies of mid and right.
#pragma omp task shared(a, tmp) firstprivate(mid, right)
    parallel(a, tmp, mid + 1, right);

    // Wait until both child tasks have completed.
    // The two halves must be sorted before they can be merged safely.
#pragma omp taskwait

    // Merge the two sorted halves into one sorted range.
    merge(a, tmp, left, mid, right);
}
}  // End of the unnamed namespace.

// Main function for Task 1.
// It tests recursive task-parallel merge sort using different array sizes
// and different numbers of OpenMP threads.
void task1() {
    // Display the task name and the cutoff used for task creation.
    std::cout << "\nTask 1: Recursive OpenMP task merge sort (cutoff="
              << cutoff << ")\n";

    // Tracks whether every correctness test passes.
    bool all = true;

    // Test merge sort with three different array sizes.
    for (int n : {100000, 1000000, 10000000}) {
        // Use a fixed random seed so the same data can be reproduced.
        // Each array size gets its own random-number generator.
        std::mt19937 rng(12345);

        // Generate random integers between -1,000,000 and 1,000,000.
        std::uniform_int_distribution<int> dist(-1000000, 1000000);

        // Create the original array containing n integers.
        std::vector<int> original(n);

        // Fill the array with randomly generated values.
        for (int& x : original) x = dist(rng);

        // Make a copy of the original array for the serial reference result.
        auto reference = original;

        // Allocate a temporary array used by merge sort.
        std::vector<int> temp(n);

        // Measure the execution time of the serial merge sort.
        // timed() is provided by the project's supporting code.
        const double serial_time = timed([&] {
            serial(reference.data(), temp.data(), 0, n - 1);
        });

        // Print the array size and serial execution time.
        // fixed and setprecision(6) display six digits after the decimal point.
        std::cout << "\nN=" << n << "  Serial=" << std::fixed
                  << std::setprecision(6) << serial_time << " s\n";

        // Print the headings for the performance results table.
        header();

        // Test the parallel algorithm with 1, 2, 4, and 8 threads.
        for (int threads : {1, 2, 4, 8}) {
            // Start every test with a fresh copy of the unsorted original data.
            auto a = original;

            // Disable dynamic adjustment of the number of OpenMP threads.
            // This requests the specified thread count for the parallel region.
            omp_set_dynamic(0);

            // Measure the execution time of the parallel merge sort.
            const double elapsed = timed([&] {
                // Create an OpenMP parallel region using the requested threads.
                // The threads can execute tasks created by the recursive function.
#pragma omp parallel num_threads(threads)
                {
                    // Only one thread initially calls the recursive function.
                    // The generated tasks can then be executed by the OpenMP team.
#pragma omp single
                    parallel(a.data(), temp.data(), 0, n - 1);
                }
            });

            // Check that the result is sorted in ascending order.
            // Also compare it with the serial result to verify exact equality.
            const bool ok = std::is_sorted(a.begin(), a.end()) && a == reference;

            // Keep the overall result as false if any test fails.
            all &= ok;

            // Print the thread count, parallel time, speedup, efficiency,
            // and whether the correctness check passed.
            row(threads, elapsed, serial_time, ok);
        }
    }

    // Display PASS only if all parallel sorting tests succeeded.
    std::cout << "Task 1 overall: " << (all ? "PASS" : "FAIL") << "\n";
}
