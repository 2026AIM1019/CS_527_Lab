
#include <omp.h>       // OpenMP functions, such as omp_get_wtime() and omp_set_num_threads()
#include <algorithm>   // Provides sort()
#include <iomanip>    // Provides setw(), fixed, and setprecision() for formatted output
#include <iostream>   // Provides cout for printing output
#include <vector>     // Provides the vector container for storing array elements

using namespace std;

// ============================================================
// FUNCTION 1: medianTime()
// Purpose: Measure the execution time of a function multiple
//          times and return the median (middle) time.
//
// Why use median?
// One run might be unusually slow because of other processes
// running on the computer. The median reduces the effect of
// such unusually fast or slow measurements.
// ============================================================

// "Function" is a template parameter, so this function can
// accept different kinds of callable objects, such as lambdas.
template <class Function>
double medianTime(Function function, int samples = 3) {

    // Perform one warm-up execution.
    // This execution is NOT included in the measured timings.
    function();

    // Store the measured execution times in a vector.
    vector<double> times;

    // Reserve space for the requested number of measurements.
    times.reserve(samples);

    // Execute and time the function "samples" times.
    for (int sample = 0; sample < samples; ++sample) {

        // Record the starting wall-clock time.
        double start = omp_get_wtime();

        // Execute the function being measured.
        function();

        // Calculate elapsed time and store it.
        // End time - start time = execution time in seconds.
        times.push_back(omp_get_wtime() - start);
    }

    // Sort the times from smallest to largest.
    sort(times.begin(), times.end());

    // Return the middle measurement.
    // For 3 samples, index 1 contains the median.
    return times[times.size() / 2];
}


// ============================================================
// FUNCTION 2: sumSerial()
// Purpose: Calculate the sum using only one thread.
//
// This is the correct reference implementation.
// We will compare the parallel reduction result against it.
// ============================================================

long long sumSerial(const vector<long long>& a) {

    // Start the total at zero.
    long long sum = 0;

    // Visit every element of the array one by one.
    // "value" represents the current element.
    for (long long value : a) {

        // Add the current element to the running total.
        // Equivalent to: sum = sum + value
        sum += value;
    }

    // Return the final total after all elements are processed.
    return sum;
}


// ============================================================
// FUNCTION 3: sumNaive()
// Purpose: Demonstrate an UNSAFE parallel sum.
//
// IMPORTANT:
// This function intentionally contains a data race.
// Do NOT use it when a correct answer is required.
//
// Every thread tries to update the SAME variable, "sum",
// without synchronization or a reduction clause.
// ============================================================

long long sumNaive(const vector<long long>& a) {

    // Shared accumulator initialized to zero.
    // All threads in the parallel region access this same sum.
    long long sum = 0;

    // Convert the array size to long long for the loop index.
    const long long n = static_cast<long long>(a.size());

    // Ask OpenMP to distribute loop iterations among threads.
    #pragma omp parallel for

    // Each thread processes some of the array elements.
    for (long long i = 0; i < n; ++i) {

        // DANGER: Multiple threads may execute this update
        // simultaneously.
        //
        // The operation sum += a[i] is not an atomic update.
        // One thread can overwrite another thread's update.
        //
        // This is a DATA RACE, so the program has undefined
        // behavior under the C++ memory model.
        //
        // We intentionally keep this broken for the experiment.
        sum += a[static_cast<size_t>(i)];
    }

    // The returned result may be incorrect or inconsistent.
    // Even if it looks correct once, the code is still unsafe.
    return sum;
}


// ============================================================
// FUNCTION 4: sumReduction()
// Purpose: Calculate the sum safely using OpenMP reduction.
//
// This is the CORRECT parallel implementation.
// ============================================================

long long sumReduction(const vector<long long>& a) {

    // The original sum starts at zero.
    long long sum = 0;

    // Get the total number of array elements.
    const long long n = static_cast<long long>(a.size());

    // OpenMP distributes the loop iterations among threads.
    //
    // reduction(+:sum) tells OpenMP to:
    // 1. Give each participating thread a private copy of sum.
    // 2. Initialize each private copy to zero for addition.
    // 3. Let each thread add its assigned elements locally.
    // 4. Combine the partial sums using the + operator.
    // 5. Store the final combined result in the original sum.
    //
    // This avoids the data race in sumNaive().
    #pragma omp parallel for reduction(+:sum)

    // Every iteration adds one array element to the thread's
    // private reduction copy of sum.
    for (long long i = 0; i < n; ++i) {
        sum += a[static_cast<size_t>(i)];
    }

    // After the reduction completes, sum contains the total.
    return sum;
}


// ============================================================
// FUNCTION 5: runTask2()
// Purpose:
// 1. Create arrays containing 10 million and 100 million values.
// 2. Calculate the serial reference sum and timing.
// 3. Test the unsafe naive sum multiple times.
// 4. Test the reduction sum at different thread counts.
// 5. Check whether the reduction result matches the serial sum.
// 6. Print the results in a table.
// ============================================================

void runTask2() {

    // The two required array sizes:
    // 10^7 = 10,000,000 elements
    // 10^8 = 100,000,000 elements
    const vector<long long> sizes = {10000000, 100000000};

    // Test the program with 1, 2, 4, and 8 threads.
    const vector<int> threadCounts = {1, 2, 4, 8};

    cout << "\nTask 2: Sum of Array Elements\n";

    // Inform the user that the naive version is intentionally
    // unsafe. Its output is for observation, not correctness.
    cout << "Naive sum is intentionally data-racy; "
         << "its results are observations only, never valid answers.\n";

    // Reduction timing uses 3 measurements after a warm-up.
    cout << "Reduction timings are medians of 3 runs after a warm-up.\n";


    // ========================================================
    // OUTER LOOP: Process each required array size.
    // First 10 million, then 100 million elements.
    // ========================================================

    for (long long n : sizes) {

        cout << "\nN = " << n << "\n";

        // Create a vector that will store n elements.
        vector<long long> a;

        // A very large allocation may fail if the computer
        // does not have enough available memory.
        try {
            a.resize(static_cast<size_t>(n));
        } catch (const bad_alloc&) {

            // If memory allocation fails, display a message
            // and move on to the next array size.
            cout << "Skipped N = " << n
                 << ": not enough memory.\n";

            continue;
        }


        // ====================================================
        // INITIALIZE THE ARRAY
        // Fill it with the repeating values:
        // 1, 2, 3, ..., 10, 11, 1, 2, ...
        //
        // i % 11 produces values from 0 through 10.
        // Adding 1 changes them to values from 1 through 11.
        // ====================================================

        for (long long i = 0; i < n; ++i) {
            a[static_cast<size_t>(i)] = (i % 11) + 1;
        }


        // ====================================================
        // STEP 1: MEASURE THE SERIAL IMPLEMENTATION
        // ====================================================

        // Measure the serial sum using the medianTime helper.
        //
        // The lambda [&]() { ... } is a small function that
        // calls sumSerial(a).
        //
        // "volatile" makes the local result observable to the
        // compiler, helping prevent elimination of the work
        // whose timing we want to measure.
        const double serialTime = medianTime(
            [&]() {
                volatile long long result = sumSerial(a);
                (void)result;
            }
        );

        // Calculate the correct reference sum.
        // This value is used to verify the reduction result.
        const long long expected = sumSerial(a);

        // Print the serial answer and its median execution time.
        cout << "Serial sum: " << expected
             << ", median time: "
             << fixed << setprecision(9)
             << serialTime << " s\n";


        // ====================================================
        // PRINT THE TABLE HEADINGS
        // Each setw() controls the width of a printed column.
        // ====================================================

        cout << left << setw(9) << "Threads"
             << right << setw(16) << "Naive run 1"
             << setw(16) << "Naive run 2"
             << setw(16) << "Naive run 3"
             << setw(18) << "Naive median(s)"
             << setw(19) << "Reduction sum"
             << setw(18) << "Reduction(s)"
             << setw(12) << "Check" << '\n';

        // Print a line to separate the headings from the data.
        cout << string(124, '-') << '\n';


        // ====================================================
        // INNER LOOP: Test each thread count.
        // For this array size, test 1, 2, 4, and 8 threads.
        // ====================================================

        for (int threads : threadCounts) {

            // Set the default number of threads for subsequent
            // OpenMP parallel regions.
            omp_set_num_threads(threads);


            // =================================================
            // STEP 2: RUN THE NAIVE VERSION THREE TIMES
            // =================================================

            // Store the three naive results.
            vector<long long> naiveResults(3);

            // Store the execution time for each naive run.
            vector<double> naiveTimes(3);

            // Repeat the unsafe calculation three times.
            for (int run = 0; run < 3; ++run) {

                // Record the start time.
                const double start = omp_get_wtime();

                // Run the intentionally broken parallel sum.
                naiveResults[run] = sumNaive(a);

                // Store the elapsed time.
                naiveTimes[run] = omp_get_wtime() - start;
            }

            // Make a copy of the naive timings so that we can
            // sort them without changing the original vector.
            vector<double> sortedTimes = naiveTimes;

            // Sort the three timing measurements.
            sort(sortedTimes.begin(), sortedTimes.end());

            // With three measurements, index 1 is the median.
            const double naiveMedian = sortedTimes[1];


            // =================================================
            // STEP 3: WARM UP THE REDUCTION IMPLEMENTATION
            // =================================================

            // Run the reduction once before timing it.
            // This result is not used for the final correctness
            // check; the purpose here is a warm-up.
            const long long reductionResult = sumReduction(a);

            // Explicitly mark the variable as intentionally
            // unused, avoiding an unused-variable warning.
            (void)reductionResult;


            // =================================================
            // STEP 4: MEASURE THE REDUCTION IMPLEMENTATION
            // =================================================

            // medianTime performs another warm-up and then
            // takes three timing measurements.
            //
            // The volatile local result helps ensure that the
            // calculation's output is used.
            const double reductionTime = medianTime(
                [&]() {
                    volatile long long result = sumReduction(a);
                    (void)result;
                }
            );


            // =================================================
            // STEP 5: VERIFY THE REDUCTION RESULT
            // =================================================

            // Calculate the reduction sum for correctness
            // checking. This call is separate from timing.
            const long long checkedResult = sumReduction(a);

            // Compare the parallel answer against the serial
            // reference answer.
            const bool ok = checkedResult == expected;


            // =================================================
            // STEP 6: PRINT THE RESULTS FOR THIS THREAD COUNT
            // =================================================

            cout << left << setw(9) << threads
                 << right << setw(16) << naiveResults[0]
                 << setw(16) << naiveResults[1]
                 << setw(16) << naiveResults[2]
                 << setw(18) << fixed << setprecision(9)
                 << naiveMedian
                 << setw(19) << checkedResult
                 << setw(18) << reductionTime
                 // Print PASS if the reduction sum matches
                 // the serial sum; otherwise, print FAIL.
                 << setw(12) << (ok ? "PASS" : "FAIL")
                 << '\n';
        }
    }


    // ========================================================
    // FINAL MESSAGE
    // A critical section could protect each shared update,
    // but only one thread could execute the protected update
    // at a time. Reduction is generally a better choice for
    // calculating a sum in parallel.
    //
    // NOTE: This program does not implement or benchmark
    // a critical-section version; it only explains the idea.
    // ========================================================

    cout << "\nA critical section around every sum update can "
         << "protect correctness, but serializes updates and "
         << "is usually slower than reduction.\n";
}


