// OpenMP header: provides functions for parallel programming,
// such as omp_get_wtime() and omp_set_num_threads().
#include <omp.h>

// Provides sort(), which is used to arrange timing measurements
// in ascending order so that we can find their median.
#include <algorithm>

// Provides output formatting tools such as setw(), fixed,
// setprecision(), left, and right.
#include <iomanip>

// Provides standard input/output objects such as cout.
#include <iostream>

// Provides bad_alloc, which is thrown when memory allocation fails.
#include <new>

// Provides length_error, which can be thrown when a vector
// is requested to have a size larger than its maximum permitted size.
#include <stdexcept>

// Provides the vector container used to store the input arrays,
// output array, and timing measurements.
#include <vector>

using namespace std;


// ============================================================
// FUNCTION: medianTime()
// PURPOSE:
//     Measures the execution time of a given function multiple
//     times and returns the median measured execution time.
//
// TEMPLATE PARAMETER:
//     Function - allows this function to accept different callable
//                objects, such as lambdas and ordinary functions.
//
// PARAMETERS:
//     function - the operation whose execution time is measured.
//     samples  - number of timed measurements to collect.
//                The default value is 3.
//
// RETURN VALUE:
//     Median execution time in seconds.
//
// WHY USE THE MEDIAN?
//     A single measurement can be affected by operating-system
//     scheduling, CPU activity, and other temporary overhead.
//     The median reduces the influence of unusually high or low
//     measurements.
// ============================================================
template <class Function>
double medianTime(Function function, int samples = 3) {

    // Execute the function once before collecting measurements.
    // This is called a warm-up run.
    //
    // The warm-up is not included in the recorded timings.
    // It can help reduce the effect of initial execution overhead.
    function();

    // Store the execution time of each measured run.
    vector<double> times;

    // Reserve enough capacity for the requested number of samples.
    // This avoids repeated vector reallocations while adding timings.
    times.reserve(samples);

    // Repeat the operation 'samples' times to collect timings.
    for (int sample = 0; sample < samples; ++sample) {

        // Record the wall-clock time immediately before the operation.
        double start = omp_get_wtime();

        // Execute the operation being benchmarked.
        function();

        // Record the elapsed time and append it to the timings vector.
        //
        // omp_get_wtime() - start gives the elapsed wall-clock time
        // in seconds, including time spent waiting or being scheduled.
        times.push_back(omp_get_wtime() - start);
    }

    // Sort all measured execution times from smallest to largest.
    sort(times.begin(), times.end());

    // Return the middle element of the sorted timings.
    //
    // With the default of 3 samples, this returns the second-smallest
    // value, which is the median of the three measurements.
    return times[times.size() / 2];
}


// ============================================================
// FUNCTION: addSerial()
// PURPOSE:
//     Adds two vectors sequentially, using a single execution
//     thread rather than explicitly distributing work across threads.
//
// PARAMETERS:
//     a - first input vector (read-only).
//     b - second input vector (read-only).
//     c - output vector that receives the sums.
//
// OPERATION:
//     c[i] = a[i] + b[i]
//
// IMPORTANT:
//     The vectors are passed by reference to avoid copying them.
//     The const keyword prevents this function from modifying a or b.
//
// ASSUMPTION:
//     All three vectors have the same size.
// ============================================================
void addSerial(
    const vector<double>& a,
    const vector<double>& b,
    vector<double>& c
) {

    // Visit each element of the input vectors in order.
    // size_t is an unsigned integer type commonly used for container sizes.
    for (size_t i = 0; i < a.size(); ++i)

        // Add the corresponding elements from a and b
        // and store the result at the same index in c.
        c[i] = a[i] + b[i];
}


// ============================================================
// FUNCTION: addParallel()
// PURPOSE:
//     Performs the same vector addition as addSerial(), but uses
//     OpenMP to distribute loop iterations across multiple threads.
//
// PARAMETERS:
//     a - first input vector (read-only).
//     b - second input vector (read-only).
//     c - output vector containing the calculated sums.
//
// OPERATION:
//     c[i] = a[i] + b[i]
//
// PARALLELIZATION:
//     The OpenMP parallel-for directive distributes loop iterations
//     among the threads in the OpenMP team.
//
// IMPORTANT:
//     Each iteration writes to a different element of c, so the
//     iterations do not conflict with one another, provided the
//     vectors have the expected sizes.
// ============================================================
void addParallel(
    const vector<double>& a,
    const vector<double>& b,
    vector<double>& c
) {

    // Convert the number of elements to long long for the loop index.
    // This supports the large vector sizes used in this benchmark.
    const long long n = static_cast<long long>(a.size());

    // OpenMP directive:
    //     parallel - creates a team of threads for the loop.
    //     for      - distributes loop iterations among those threads.
    //
    // The number of threads is influenced by the OpenMP settings,
    // including omp_set_num_threads(), called later in runTask1().
    //
    // OpenMP handles assigning iterations to the available threads.
    #pragma omp parallel for
    for (long long i = 0; i < n; ++i)

        // Calculate the sum for this index and store it in c.
        //
        // static_cast<size_t>(i) converts the signed loop index
        // to the index type used by vector.
        //
        // Each iteration handles a separate index, allowing
        // multiple iterations to execute concurrently.
        c[static_cast<size_t>(i)] =
            a[static_cast<size_t>(i)] + b[static_cast<size_t>(i)];
}


// ============================================================
// FUNCTION: verifyAdd()
// PURPOSE:
//     Checks whether the output vector contains the correct
//     element-by-element sums of the two input vectors.
//
// PARAMETERS:
//     a - first input vector.
//     b - second input vector.
//     c - output vector to validate.
//
// RETURN VALUE:
//     true  - all sizes match and every computed sum is correct.
//     false - the sizes differ or at least one sum is incorrect.
//
// WHY IS THIS IMPORTANT?
//     A benchmark must check correctness as well as performance.
//     A faster implementation is not useful if it produces wrong
//     results.
// ============================================================
bool verifyAdd(
    const vector<double>& a,
    const vector<double>& b,
    const vector<double>& c
) {

    // First, confirm that all vectors contain the same number
    // of elements. If not, the result cannot be validated safely.
    if (a.size() != b.size() || a.size() != c.size())
        return false;

    // Check each output element against the expected sum.
    for (size_t i = 0; i < a.size(); ++i) {

        // If even one output element differs from a[i] + b[i],
        // the verification fails immediately.
        if (c[i] != a[i] + b[i])
            return false;
    }

    // Reaching this point means every element passed the check.
    return true;
}


// ============================================================
// FUNCTION: runTask1()
// PURPOSE:
//     Benchmarks serial and parallel vector addition for multiple
//     input sizes and multiple OpenMP thread counts.
//
// EXPERIMENTS:
//     Array sizes: 100,000; 1,000,000; 10,000,000; 100,000,000
//     Thread counts: 1, 2, 4, 8
//
// FOR EACH ARRAY SIZE, THIS FUNCTION:
//     1. Allocates the input and output vectors.
//     2. Initializes the input vectors.
//     3. Measures serial vector addition.
//     4. Verifies the serial result.
//     5. Measures parallel vector addition at different thread counts.
//     6. Verifies each parallel result.
//     7. Calculates and prints the speedup.
//
// SPEEDUP FORMULA:
//     Speedup = Serial execution time / Parallel execution time
//
// A speedup greater than 1 means the parallel version was faster
// than the serial baseline for that measurement.
// ============================================================
void runTask1() {

    // Define the problem sizes to test.
    //
    // Each value represents the number of elements in each vector.
    // The largest test contains 100 million elements per vector.
    const vector<long long> sizes = {
        100000, 1000000, 10000000, 100000000
    };

    // Define the OpenMP thread counts to test for every problem size.
    const vector<int> threadCounts = {1, 2, 4, 8};

    // Print the task heading and explain how timings are obtained.
    cout << "\nTask 1: Vector Addition\n";

    // Each reported timing is the median of three measured runs
    // following one warm-up run, as implemented by medianTime().
    cout << "Times use omp_get_wtime(); each timing is the median of 3 runs after a warm-up.\n";


    // ------------------------------------------------------------
    // OUTER LOOP:
    // Repeat the complete benchmark for every vector size.
    // The variable n is the number of elements in each vector.
    // ------------------------------------------------------------
    for (long long n : sizes) {

        // Print the size of the current experiment.
        cout << "\nN = " << n << "\n";

        // Create three initially empty vectors:
        // a = first input, b = second input, c = output.
        vector<double> a, b, c;

        // Attempt to allocate enough storage for all three vectors.
        // The try block allows allocation-related failures to be
        // handled without terminating the entire benchmark.
        try {

            // Resize each vector to contain n double elements.
            // static_cast<size_t>(n) converts n to the type
            // conventionally used for container sizes.
            a.resize(static_cast<size_t>(n));
            b.resize(static_cast<size_t>(n));
            c.resize(static_cast<size_t>(n));

        } catch (const bad_alloc&) {

            // bad_alloc indicates that a memory allocation failed.
            // Print a message and skip the remaining work for this n.
            cout << "Skipped N = " << n
                 << ": not enough memory to allocate arrays.\n";

            // Continue with the next value in the outer sizes loop.
            continue;

        } catch (const length_error&) {

            // length_error indicates that the requested vector size
            // exceeds the vector's permitted maximum size.
            cout << "Skipped N = " << n
                 << ": requested array size is too large.\n";

            // Skip this problem size and proceed to the next one.
            continue;
        }


        // --------------------------------------------------------
        // INITIALIZE THE INPUT VECTORS
        //
        // Fill a and b with deterministic values so the benchmark
        // always uses reproducible input data.
        //
        // The modulo operations keep the values within a bounded
        // range even when n is very large.
        // --------------------------------------------------------
        for (long long i = 0; i < n; ++i) {

            // Generate the first input value.
            //
            // i % 1000 produces a remainder from 0 to 999.
            // Convert it to double and multiply by 0.5.
            a[static_cast<size_t>(i)] =
                static_cast<double>(i % 1000) * 0.5;

            // Generate the second input value.
            //
            // i % 997 produces a remainder from 0 to 996.
            // Convert it to double and multiply by 0.25.
            b[static_cast<size_t>(i)] =
                static_cast<double>(i % 997) * 0.25;
        }


        // --------------------------------------------------------
        // SERIAL BENCHMARK
        // --------------------------------------------------------

        // Measure the serial addition using medianTime().
        //
        // The lambda captures a, b, and c by reference and calls
        // addSerial() whenever medianTime() invokes it.
        //
        // medianTime() performs one warm-up and then three
        // timed runs by default.
        const double serialTime =
            medianTime([&]() { addSerial(a, b, c); });

        // Check whether the serial implementation computed
        // every element correctly.
        const bool serialOk = verifyAdd(a, b, c);

        // Print the median serial execution time in seconds
        // and indicate whether the result passed verification.
        cout << "Serial median time: " << serialTime
             << " s, check: "
             << (serialOk ? "PASS" : "FAIL") << "\n";


        // --------------------------------------------------------
        // PRINT THE PARALLEL RESULTS TABLE HEADER
        // --------------------------------------------------------

        // left and right control text alignment.
        // setw() sets the minimum field width for the next item.
        //
        // The columns are:
        //     Threads      - requested number of OpenMP threads
        //     Parallel (s) - median parallel execution time
        //     Check        - correctness status
        //     Speedup      - serial time divided by parallel time
        cout << left << setw(10) << "Threads"
             << right << setw(18) << "Parallel (s)"
             << setw(14) << "Check"
             << setw(16) << "Speedup" << '\n';

        // Print a separator line containing 58 hyphens.
        cout << string(58, '-') << '\n';


        // --------------------------------------------------------
        // INNER LOOP:
        // Run the parallel benchmark with each requested thread
        // count: 1, 2, 4, and 8.
        //
        // This loop runs separately for every successfully
        // allocated problem size.
        // --------------------------------------------------------
        for (int threads : threadCounts) {

            // Request the specified number of OpenMP threads
            // for subsequent parallel regions using the default
            // thread-count setting.
            //
            // The actual number of threads used can depend on
            // OpenMP runtime settings and execution constraints.
            omp_set_num_threads(threads);


            // Measure parallel vector addition.
            //
            // addParallel() uses '#pragma omp parallel for' to
            // distribute loop iterations among OpenMP threads.
            //
            // The timing includes the overhead occurring inside
            // the measured addParallel() call, including relevant
            // parallel-region overhead.
            const double parallelTime =
                medianTime([&]() { addParallel(a, b, c); });


            // Verify the result produced by the parallel operation.
            // Store true if all elements match their expected sums.
            const bool ok = verifyAdd(a, b, c);


            // ----------------------------------------------------
            // CALCULATE SPEEDUP
            //
            // Speedup = serialTime / parallelTime
            //
            // Example:
            //     Serial time   = 2.0 seconds
            //     Parallel time = 0.5 seconds
            //     Speedup       = 2.0 / 0.5 = 4.0
            //
            // This means the parallel operation took one-quarter
            // of the serial execution time.
            //
            // Check parallelTime > 0.0 to avoid division by zero.
            // If it is zero or negative, assign speedup = 0.0.
            // ----------------------------------------------------
            const double speedup =
                parallelTime > 0.0
                    ? serialTime / parallelTime
                    : 0.0;


            // ----------------------------------------------------
            // PRINT THE RESULTS FOR THE CURRENT THREAD COUNT
            // ----------------------------------------------------

            // Print the requested number of threads first.
            cout << left << setw(10) << threads

                 // Print parallel execution time using fixed-point
                 // notation with nine digits after the decimal point.
                 << right << setw(18) << fixed
                 << setprecision(9) << parallelTime

                 // Display PASS only if both the parallel result
                 // and the serial baseline passed verification.
                 // The && operator requires both conditions to
                 // be true for PASS to be printed.
                 << setw(14) << (ok && serialOk ? "PASS" : "FAIL")

                 // Print speedup with three digits after the
                 // decimal point. The fixed formatting set above
                 // remains active for this floating-point value.
                 << setw(16) << setprecision(3) << speedup

                 // Finish the current row and move to the next line.
                 << '\n';
        }

        // The inner loop has finished testing all thread counts.
        // The outer loop now proceeds to the next vector size.
    }

    // Once every problem size has been processed or skipped,
    // the Task 1 benchmark is complete.
}

