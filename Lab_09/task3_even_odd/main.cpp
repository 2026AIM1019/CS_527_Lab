
#include "../common.h"

// An anonymous namespace keeps the definitions inside it local to this
// source file. These functions and the Counts structure are not intended
// to be accessed directly from other source files.
namespace {

// Structure to store the number of even and odd integers.
//
// Using a structure allows both counters to be returned together from
// the serial function and the parallel implementations.
//
// long long is used for the counters because the input may contain
// millions of elements, so the counts can become quite large.
struct Counts {
    // Stores the total number of even integers.
    // Initialized to zero so counting can begin without extra initialization.
    long long even = 0;

    // Stores the total number of odd integers.
    // This counter also starts at zero.
    long long odd = 0;

    // Overload the equality operator to compare two Counts objects.
    //
    // This makes it possible to write:
    // actual == expected
    //
    // The comparison returns true only when both the even counts
    // and the odd counts are equal.
    bool operator==(const Counts& other) const {
        return even == other.even && odd == other.odd;
    }
};

// Serial reference implementation.
//
// This function counts even and odd integers using a normal sequential
// loop without OpenMP parallelization.
//
// Its output acts as the reference result against which the parallel
// implementations are checked.
Counts serial(const std::vector<int>& a) {

    // Create a Counts object.
    // Both even and odd counters initially contain zero.
    Counts counts;

    // Visit each integer in the input vector exactly once.
    // The range-based loop assigns the current element to value.
    for (int value : a) {

        // An integer is even if dividing it by 2 leaves remainder zero.
        // Increase the even counter when this condition is true.
        if (value % 2 == 0) ++counts.even;

        // If the condition is false, the integer is odd.
        // Increase the odd counter instead.
        else ++counts.odd;
    }

    // Return both final counts to the calling function.
    return counts;
}

// OpenMP reduction implementation.
//
// This function performs the same counting operation as serial(),
// but distributes the loop iterations among multiple threads.
//
// The reduction clause creates private copies of the specified counters
// for the parallel computation. OpenMP combines those private values
// into the original variables when the parallel loop finishes.
//
// This avoids requiring an atomic operation every time an element is
// classified as even or odd.
Counts reduction(const std::vector<int>& a, int threads) {

    // Shared result variables used by the reduction operation.
    // OpenMP's reduction clause handles their private copies and combines
    // the partial results when the parallel loop completes.
    long long even = 0;
    long long odd = 0;

    // Get the number of elements in the vector.
    //
    // Convert the size to long long so it can be used with the long long
    // loop index below.
    const long long n = static_cast<long long>(a.size());

    // OpenMP parallelizes the following for loop.
    //
    // reduction(+:even,odd):
    //   Each thread gets private copies of even and odd.
    //   The + operator combines the partial counts at the end.
    //
    // num_threads(threads):
    //   Requests the specified number of threads for this parallel region.
    //
    // The loop index i is handled by the OpenMP loop construct.
#pragma omp parallel for reduction(+:even,odd) num_threads(threads)

    // Visit every element of the input vector.
    // OpenMP distributes these iterations across the available threads.
    for (long long i = 0; i < n; ++i) {

        // Check whether the current element is even.
        // Increment this thread's private even counter when it is even.
        if (a[i] % 2 == 0) ++even;

        // Otherwise, increment this thread's private odd counter.
        else ++odd;
    }

    // Once the parallel loop finishes, OpenMP has combined the partial
    // counts into even and odd.
    //
    // Return both totals using aggregate initialization of Counts.
    return {even, odd};
}

// Block-aggregated atomic implementation.
//
// Instead of updating shared counters for every element, this function
// divides the input into blocks of up to 4096 elements.
//
// Each block counts its even and odd elements using local counters.
// Once the block has been counted, it adds its two totals to the shared
// counters using atomic operations.
//
// This reduces the number of atomic updates compared with using an atomic
// operation for every individual input element.
Counts atomic_blocks(const std::vector<int>& a, int threads) {

    // Shared counters that store the final totals from all blocks.
    // They start at zero before the parallel work begins.
    long long even = 0;
    long long odd = 0;

    // Store the total number of input elements.
    // The conversion makes its type consistent with the loop indices.
    const long long n = static_cast<long long>(a.size());

    // Calculate how many blocks are required.
    //
    // Each block can process 4096 elements.
    //
    // Adding 4095 before dividing by 4096 performs integer ceiling
    // division, ensuring that a final partially filled block is included.
    //
    // For example:
    //   n = 4096  -> 1 block
    //   n = 4097  -> 2 blocks
    //   n = 8192  -> 2 blocks
    const long long blocks = (n + 4095) / 4096;

    // Parallelize the loop over blocks rather than individual elements.
    //
    // Each loop iteration processes one block. OpenMP distributes
    // different blocks among the requested number of threads.
    //
    // num_threads(threads) requests the specified thread count.
#pragma omp parallel for num_threads(threads)

    // Process each block of the input vector.
    for (long long b = 0; b < blocks; ++b) {

        // These local counters store the number of even and odd integers
        // found in the current block.
        //
        // Each block iteration has its own local variables, so different
        // block iterations do not share these counters.
        long long local_even = 0;
        long long local_odd = 0;

        // Calculate the exclusive ending index of the current block.
        //
        // (b + 1) * 4096 is the index immediately after the block's
        // normal ending position.
        //
        // std::min ensures that end never exceeds n. This is important
        // for the last block when fewer than 4096 elements remain.
        const long long end = std::min(n, (b + 1) * 4096);

        // Count even and odd values within the current block.
        //
        // The starting index of block b is b * 4096.
        // The loop stops before end, so it processes only valid indices.
        //
        // These updates affect only local counters. No atomic operations
        // or locks are needed for local_even and local_odd.
        for (long long i = b * 4096; i < end; ++i) {

            // If the current integer is even, increment the local
            // even counter for this block.
            if (a[i] % 2 == 0) ++local_even;

            // Otherwise, increment the local odd counter.
            else ++local_odd;
        }

        // The block has now been counted.
        //
        // Multiple blocks may finish at approximately the same time.
        // Since they all update the shared even counter, an ordinary
        // non-atomic addition could cause updates to be lost.
        //
        // The atomic directive makes this shared update safe:
        // OpenMP ensures that the addition is performed atomically.
#pragma omp atomic update
        even += local_even;

        // Add this block's odd count to the shared odd counter.
        //
        // This update also needs to be atomic because other threads
        // may be updating odd at the same time.
#pragma omp atomic update
        odd += local_odd;
    }

    // The parallel loop has completed, so all blocks have contributed
    // their local counts to the shared counters.
    //
    // Return the final even and odd totals.
    return {even, odd};
}

}  // End of the anonymous namespace.

// Main function for Task 3.
//
// This task compares two ways of counting even and odd integers in parallel:
// 1. OpenMP reduction using two counters.
// 2. Atomic updates after counting each block locally.
//
// A sequential implementation provides the reference output.
// The program also measures execution times and checks correctness
// for different input sizes and thread counts.
void task3() {

    // Display the task name and describe the two parallel methods
    // being compared.
    std::cout << "\nTask 3: Multiple reduction vs block-aggregated atomic updates\n";

    // Tracks whether every correctness check passes.
    // The value remains true only if all tests succeed.
    bool all = true;

    // Run the experiment with four different input sizes.
    //
    // Larger inputs help demonstrate how the two parallel approaches
    // behave as the amount of work increases.
    for (int n : {100000, 1000000, 10000000, 100000000}) {

        // Create a vector containing n integers.
        // This vector is the input for the serial and parallel functions.
        std::vector<int> a(n);

        // Generate deterministic input values.
        //
        // The expression uses the loop index to calculate each element.
        // The modulo operation keeps every value within the range
        // 0 through 10006.
        //
        // Because the input is deterministic, each run generates the
        // same sequence of values for a given input size.
        //
        // The generated sequence contains both even and odd integers.
        for (int i = 0; i < n; ++i) {
            a[i] = (i * 17LL + 23) % 10007;
        }

        // This object will store the expected counts from the serial
        // implementation.
        Counts expected;

        // Measure the time taken by the sequential implementation.
        //
        // The lambda calls serial(a) and stores its returned Counts object
        // in expected.
        //
        // The timed() function returns the elapsed execution time.
        const double baseline = timed([&] { expected = serial(a); });

        // Display the current input size, sequential execution time,
        // and the reference counts for even and odd integers.
        //
        // std::fixed and std::setprecision(6) format the execution time
        // to six digits after the decimal point.
        std::cout << "\nN=" << n << "  Serial=" << std::fixed
                  << std::setprecision(6) << baseline << " s; even="
                  << expected.even << ", odd=" << expected.odd << "\n";

        // Compare the two parallel counting methods.
        //
        // mode == 0 selects the OpenMP reduction implementation.
        // mode == 1 selects the block-aggregated atomic implementation.
        for (int mode = 0; mode < 2; ++mode) {

            // Print the name of the method being tested.
            //
            // The first method uses a reduction clause to combine
            // thread-local counts.
            //
            // The second method counts each block locally and then
            // uses two atomic additions to update shared totals.
            std::cout << (mode == 0 ? "reduction(+:even,odd)" :
                                      "atomic (per-block aggregated updates)")
                      << '\n';

            // Display the heading for the timing results table.
            // The header() function is defined elsewhere in the project.
            header();

            // Test the selected method with different numbers of threads.
            //
            // Comparing 1, 2, 4, and 8 threads helps show how execution
            // time changes as more threads are requested.
            for (int threads : {1, 2, 4, 8}) {

                // This object stores the counts produced by the selected
                // parallel implementation.
                Counts actual;

                // Disable automatic adjustment of the OpenMP thread count.
                // This allows the experiment to request the intended number
                // of threads explicitly.
                omp_set_dynamic(0);

                // Measure the execution time of the selected parallel method.
                const double elapsed = timed([&] {

                    // Select the implementation according to mode.
                    //
                    // If mode is zero, call reduction().
                    // Otherwise, call atomic_blocks().
                    //
                    // Both functions receive the same input vector and
                    // requested thread count, allowing their results
                    // and execution times to be compared.
                    actual = (mode == 0) ? reduction(a, threads)
                                         : atomic_blocks(a, threads);
                });

                // Verify the correctness of the parallel result.
                //
                // First condition:
                // actual == expected compares both the even and odd counts
                // against the counts calculated by the serial implementation.
                //
                // Second condition:
                // actual.even + actual.odd == n checks that every input
                // element has been counted exactly once across the two
                // categories.
                //
                // Both conditions must be true for the test to pass.
                const bool ok = actual == expected &&
                                actual.even + actual.odd == n;

                // Update the overall result of the task.
                //
                // If any individual test fails, all becomes false and
                // remains false for the rest of the experiment.
                all &= ok;

                // Print the number of threads, parallel execution time,
                // serial baseline time, and correctness status.
                //
                // The row() function is defined elsewhere in the project
                // and is responsible for displaying one row of results.
                row(threads, elapsed, baseline, ok);
            }
        }
    }

    // Print the final correctness status after all experiments finish.
    //
    // PASS means every parallel implementation produced the expected
    // even and odd counts for every input size and thread count.
    //
    // FAIL means at least one correctness check did not pass.
    std::cout << "Task 3 overall: " << (all ? "PASS" : "FAIL") << "\n";
}
