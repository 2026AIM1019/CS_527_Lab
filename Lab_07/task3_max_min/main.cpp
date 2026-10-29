// OpenMP header provides functions for parallel programming,
// such as getting the current thread number, setting the number
// of threads, and measuring execution time.
#include <omp.h>

// Provides the sort() function, which is used to sort execution times.
#include <algorithm>

// Provides input and output operations such as cout.
#include <iomanip>

// Provides cout for printing results to the terminal.
#include <iostream>

// Provides numeric_limits, which gives the minimum and maximum
// possible values for data types such as int and long long.
#include <limits>

// Provides the vector container, which is used to store array
// elements, execution times, and thread-local results.
#include <vector>

// Allows standard library names to be used without writing std::.
using namespace std;


// This is a template function that measures how long a function takes
// to execute and returns the median execution time.
//
// Function:
//   function - The function whose execution time must be measured.
//   samples  - The number of timed executions. The default is 3.
//
// Working:
//   1. Execute the function once before measuring its time.
//      This acts as a warm-up run.
//   2. Execute the function repeatedly and record its execution time.
//   3. Sort all recorded execution times.
//   4. Return the middle value as the median.
//
// The median helps reduce the effect of unusually high or low
// execution times caused by other system activities.
template <class Function>
double medianTime(Function function, int samples = 3) {

    // Warm-up execution: run the function once without recording
    // its execution time.
    function();

    // Store the execution time of each measured run.
    vector<double> times;

    // Reserve enough memory for the requested number of samples.
    // This avoids unnecessary memory allocations while recording times.
    times.reserve(samples);

    // Repeat the measurement 'samples' times.
    for (int sample = 0; sample < samples; ++sample) {

        // Record the starting time using OpenMP's wall-clock timer.
        double start = omp_get_wtime();

        // Execute the function being measured.
        function();

        // Calculate the elapsed time and store it in seconds.
        times.push_back(omp_get_wtime() - start);
    }

    // Arrange the execution times in ascending order.
    sort(times.begin(), times.end());

    // Return the middle value from the sorted list.
    // For an odd number of samples, this is the exact middle element.
    return times[times.size() / 2];
}


// This structure stores the maximum and minimum values of an array
// along with the indices where those values occur.
//
// maxValue: The largest element in the array.
// maxIndex: The index of the largest element.
// minValue: The smallest element in the array.
// minIndex: The index of the smallest element.
//
// The indices use long long so they can represent large array positions.
struct MinMaxResult {
    int maxValue;
    long long maxIndex;
    int minValue;
    long long minIndex;
};


// This function finds the maximum and minimum values and their indices
// using a normal sequential, or serial, approach.
//
// Only one execution thread processes the array.
//
// Parameter:
//   a - The input array, passed by constant reference to avoid copying.
//
// Return:
//   A MinMaxResult structure containing both extreme values
//   and their corresponding indices.
MinMaxResult serialMinMax(const vector<int>& a) {

    // Initially, assume that the first array element is both
    // the maximum and the minimum.
    //
    // Both indices are set to 0 because the first element is
    // located at index 0.
    MinMaxResult r{a[0], 0, a[0], 0};

    // Start from index 1 because index 0 has already been considered.
    // The loop continues until every remaining element is checked.
    for (long long i = 1; i < static_cast<long long>(a.size()); ++i) {

        // Read the current array element.
        // static_cast<size_t>(i) converts the index to the type
        // commonly used by vector for indexing.
        const int value = a[static_cast<size_t>(i)];

        // If the current element is greater than the maximum found
        // so far, update the maximum value and its index.
        //
        // If two elements have the same maximum value, this condition
        // does not update the index. Therefore, the first occurrence
        // of the maximum value is retained.
        if (value > r.maxValue) { r.maxValue = value; r.maxIndex = i; }

        // If the current element is smaller than the minimum found
        // so far, update the minimum value and its index.
        //
        // Equal minimum values do not replace the existing index,
        // so the smallest index is retained.
        if (value < r.minValue) { r.minValue = value; r.minIndex = i; }
    }

    // Return the maximum, minimum, and their indices.
    return r;
}


// This function calculates only the maximum and minimum VALUES
// using OpenMP parallel reduction.
//
// Unlike serialMinMax(), this function does not calculate indices.
//
// Parameters:
//   a        - The input array.
//   maxValue - Output variable for the maximum array value.
//   minValue - Output variable for the minimum array value.
//
// OpenMP reduction creates private copies of the reduction variables
// for parallel execution and combines their results at the end.
void parallelValues(const vector<int>& a, int& maxValue, int& minValue) {

    // Start with the smallest possible int value for the maximum.
    // Any array element will be greater than or equal to this value.
    maxValue = numeric_limits<int>::min();

    // Start with the largest possible int value for the minimum.
    // Any array element will be smaller than or equal to this value.
    minValue = numeric_limits<int>::max();

    // Store the array length in a long long variable.
    const long long n = static_cast<long long>(a.size());

    // OpenMP parallel for divides the loop iterations among threads.
    //
    // reduction(max:maxValue):
    //   Each thread maintains a private maximum value.
    //   The private results are combined using the maximum operation.
    //
    // reduction(min:minValue):
    //   Each thread maintains a private minimum value.
    //   The private results are combined using the minimum operation.
    //
    // The reduction operations avoid data races on these variables.
    #pragma omp parallel for reduction(max:maxValue) reduction(min:minValue)
    for (long long i = 0; i < n; ++i) {

        // Read the element assigned to this loop iteration.
        const int value = a[static_cast<size_t>(i)];

        // Update the maximum value in this thread's reduction copy.
        if (value > maxValue) maxValue = value;

        // Update the minimum value in this thread's reduction copy.
        if (value < minValue) minValue = value;
    }

    // After the parallel loop finishes, OpenMP combines the
    // thread-local results and stores the final extrema in maxValue
    // and minValue.
}


// This function finds the indices of the maximum and minimum elements
// using a critical section.
//
// The loop iterations are distributed among OpenMP threads.
// However, the comparisons and updates are protected by a critical
// section, so only one thread can perform that section at a time.
//
// Parameters:
//   a        - The input array.
//   maxIndex - Output index of the maximum element.
//   minIndex - Output index of the minimum element.
void indicesCritical(const vector<int>& a, long long& maxIndex, long long& minIndex) {

    // Initially, use the first element as both the maximum
    // and minimum value.
    int bestMax = a[0], bestMin = a[0];

    // Initially, both extreme values are assumed to occur at index 0.
    maxIndex = 0;
    minIndex = 0;

    // Store the total number of array elements.
    const long long n = static_cast<long long>(a.size());

    // Distribute loop iterations among the available OpenMP threads.
    // The loop begins at index 1 because index 0 initializes the values.
    #pragma omp parallel for
    for (long long i = 1; i < n; ++i) {

        // A critical section ensures that only one thread at a time
        // can execute the enclosed statements.
        //
        // This protects bestMax, bestMin, maxIndex, and minIndex
        // from simultaneous updates by multiple threads.
        //
        // The disadvantage is that threads may spend time waiting
        // for other threads to finish this section.
        #pragma omp critical
        {

            // Read the current array element inside the critical section.
            const int value = a[static_cast<size_t>(i)];

            // Update the maximum if:
            //   1. The current value is larger than the current maximum.
            //   2. The values are equal, but the current index is smaller.
            //
            // The second condition ensures that ties select the
            // smallest index.
            if (value > bestMax || (value == bestMax && i < maxIndex)) {
                bestMax = value;
                maxIndex = i;
            }

            // Update the minimum if:
            //   1. The current value is smaller than the current minimum.
            //   2. The values are equal, but the current index is smaller.
            //
            // This also ensures that the first occurrence is selected
            // when multiple elements share the same minimum value.
            if (value < bestMin || (value == bestMin && i < minIndex)) {
                bestMin = value;
                minIndex = i;
            }
        }
    }

    // When the parallel loop completes, maxIndex and minIndex
    // contain the indices of the maximum and minimum elements.
}


// This function finds the indices of the maximum and minimum elements
// using manual thread-local results.
//
// Instead of updating shared maximum and minimum indices for every
// array element, each thread calculates its own local extrema.
//
// After all threads finish, a serial loop combines the local results.
//
// This approach reduces synchronization during the main parallel loop.
//
// Parameters:
//   a        - The input array.
//   maxIndex - Output index of the maximum element.
//   minIndex - Output index of the minimum element.
void indicesManual(const vector<int>& a, long long& maxIndex, long long& minIndex) {

    // Obtain the maximum number of OpenMP threads that can be used
    // for the upcoming parallel region.
    const int capacity = omp_get_max_threads();

    // Create one local maximum value for each possible thread.
    // Initially, each local maximum is the smallest possible int.
    vector<int> localMax(static_cast<size_t>(capacity), numeric_limits<int>::min());

    // Create one local minimum value for each possible thread.
    // Initially, each local minimum is the largest possible int.
    vector<int> localMin(static_cast<size_t>(capacity), numeric_limits<int>::max());

    // Store the index of each thread's local maximum.
    //
    // Initially, use the largest possible long long value as a
    // placeholder indicating that no valid index has been recorded.
    vector<long long> localMaxIndex(static_cast<size_t>(capacity), numeric_limits<long long>::max());

    // Store the index of each thread's local minimum.
    vector<long long> localMinIndex(static_cast<size_t>(capacity), numeric_limits<long long>::max());

    // Store the number of array elements.
    const long long n = static_cast<long long>(a.size());

    // Begin the OpenMP parallel region.
    // Each participating thread executes the enclosed block.
    #pragma omp parallel
    {

        // Obtain the unique ID of the current thread.
        // Thread IDs normally range from 0 to the number of threads - 1.
        //
        // This ID is used to select the current thread's own entries
        // in the localMax, localMin, localMaxIndex, and localMinIndex
        // vectors.
        const int tid = omp_get_thread_num();

        // Divide the loop iterations among the threads.
        //
        // Each thread updates only its own local result entries.
        // Different threads therefore do not update the same local
        // maximum, minimum, or index entries.
        #pragma omp for
        for (long long i = 0; i < n; ++i) {

            // Read the current array element.
            const int value = a[static_cast<size_t>(i)];

            // Update this thread's local maximum if:
            //   1. The current element is larger than its local maximum.
            //   2. The values are equal and the current index is smaller.
            //
            // This keeps the smallest index when maximum values are tied.
            if (value > localMax[tid] || (value == localMax[tid] && i < localMaxIndex[tid])) {
                localMax[tid] = value;
                localMaxIndex[tid] = i;
            }

            // Update this thread's local minimum if:
            //   1. The current element is smaller than its local minimum.
            //   2. The values are equal and the current index is smaller.
            //
            // This keeps the smallest index when minimum values are tied.
            if (value < localMin[tid] || (value == localMin[tid] && i < localMinIndex[tid])) {
                localMin[tid] = value;
                localMinIndex[tid] = i;
            }
        }

        // Threads reach the end of the parallel region after their
        // assigned loop iterations have completed.
    }

    // At this point, each thread's local results have been calculated.
    // Now combine these local results into the final maximum and minimum.

    // Initially, set the final indices to the largest possible value.
    // A valid array index will be smaller than this initial value.
    maxIndex = numeric_limits<long long>::max();
    minIndex = numeric_limits<long long>::max();

    // Initialize the final maximum and minimum values.
    int bestMax = numeric_limits<int>::min();
    int bestMin = numeric_limits<int>::max();

    // Examine the local results produced by all possible thread slots.
    for (int t = 0; t < capacity; ++t) {

        // Select the largest local maximum.
        //
        // If two threads found the same maximum value, select the
        // smaller index so that the overall tie-breaking rule is kept.
        if (localMax[t] > bestMax || (localMax[t] == bestMax && localMaxIndex[t] < maxIndex)) {
            bestMax = localMax[t];
            maxIndex = localMaxIndex[t];
        }

        // Select the smallest local minimum.
        //
        // If two threads found the same minimum value, select the
        // smaller index.
        if (localMin[t] < bestMin || (localMin[t] == bestMin && localMinIndex[t] < minIndex)) {
            bestMin = localMin[t];
            minIndex = localMinIndex[t];
        }
    }

    // After this serial combination step, maxIndex and minIndex
    // contain the final indices of the array's maximum and minimum.
}


// This function checks whether the indices returned by a parallel
// algorithm agree with the results obtained from the serial algorithm.
//
// Parameters:
//   a         - The input array.
//   expected  - The trusted result produced by serialMinMax().
//   maxIndex  - Maximum index returned by the parallel algorithm.
//   minIndex  - Minimum index returned by the parallel algorithm.
//
// Return:
//   true  - If the indices are valid and match the expected results.
//   false - If any validation condition fails.
bool validIndices(const vector<int>& a, const MinMaxResult& expected, long long maxIndex, long long minIndex) {

    // All conditions must be true for the validation to pass.
    return maxIndex >= 0 && minIndex >= 0 &&

           // Check that the maximum index is within the array bounds.
           static_cast<size_t>(maxIndex) < a.size() &&

           // Check that the minimum index is within the array bounds.
           static_cast<size_t>(minIndex) < a.size() &&

           // Check that the maximum index points to the expected
           // maximum value.
           a[static_cast<size_t>(maxIndex)] == expected.maxValue &&

           // Check that the minimum index points to the expected
           // minimum value.
           a[static_cast<size_t>(minIndex)] == expected.minValue &&

           // Check that the maximum index is exactly the same as
           // the serial result, including the smallest-index rule.
           maxIndex == expected.maxIndex &&

           // Check that the minimum index is exactly the same as
           // the serial result, including the smallest-index rule.
           minIndex == expected.minIndex;
}


// This function runs Task 3, which compares serial and parallel
// methods for finding the maximum and minimum of a large array.
//
// It performs the following operations:
//   1. Defines the array sizes and thread counts to test.
//   2. Generates deterministic array values.
//   3. Calculates the correct answer using the serial algorithm.
//   4. Measures the serial execution time.
//   5. Runs and measures the parallel algorithms.
//   6. Checks that all algorithms return correct results.
//   7. Prints a timing and correctness comparison.
void runTask3() {

    // Define the array sizes used for the experiment.
    // These represent arrays containing 10 million and 100 million
    // integer elements, respectively.
    const vector<long long> sizes = {10000000, 100000000};

    // Define the number of OpenMP threads used in each experiment.
    const vector<int> threadCounts = {1, 2, 4, 8};

    // Print the task heading.
    cout << "\nTask 3: Maximum and Minimum of an Array\n";

    // Explain how execution times and equal values are handled.
    cout << "Timings are medians of 3 runs after a warm-up; ties select the smallest index.\n";

    // Run the experiment for each array size.
    for (long long n : sizes) {

        // Print the size of the current array.
        cout << "\nN = " << n << "\n";

        // Declare the vector that will hold the input array.
        vector<int> a;

        // Attempt to allocate enough memory for all array elements.
        // A large array may not fit in the available system memory.
        try {
            a.resize(static_cast<size_t>(n));
        }

        // If memory allocation fails with bad_alloc, report the issue
        // and move on to the next array size.
        catch (const bad_alloc&) {
            cout << "Skipped N = " << n << ": not enough memory.\n";
            continue;
        }

        // Fill the array with deterministic integer values.
        //
        // The expression uses the current index to generate a value.
        // The modulo operation keeps the generated range bounded,
        // and subtracting 500001 shifts the values into a range
        // containing both negative and positive numbers.
        //
        // Since the formula is deterministic, the same array size
        // generates the same sequence of values on each run.
        for (long long i = 0; i < n; ++i)
            a[static_cast<size_t>(i)] = static_cast<int>((i * 37 + 13) % 1000003) - 500001;

        // Calculate the correct maximum, minimum, and their indices
        // using the serial algorithm.
        //
        // This result acts as the reference for checking the parallel
        // algorithms.
        const MinMaxResult expected = serialMinMax(a);

        // Measure the serial algorithm's execution time.
        //
        // The lambda function calls serialMinMax().
        // volatile and the explicit void cast are used here to retain
        // the computed result without printing it during each run.
        const double serialTime = medianTime([&]() { volatile MinMaxResult r = serialMinMax(a); (void)r; });

        // Print the serial maximum value, its index, the minimum value,
        // its index, and the median execution time.
        //
        // fixed and setprecision(9) display the time with nine digits
        // after the decimal point.
        cout << "Serial max = " << expected.maxValue << " at index " << expected.maxIndex
             << "; min = " << expected.minValue << " at index " << expected.minIndex
             << "; median time = " << fixed << setprecision(9) << serialTime << " s\n";

        // Print the column headings for the parallel timing table.
        //
        // left and right control text alignment.
        // setw() specifies the minimum width of each printed column.
        cout << left << setw(9) << "Threads"
             << right << setw(18) << "Values (s)"
             << setw(20) << "Critical idx (s)"
             << setw(19) << "Manual idx (s)"
             << setw(12) << "Check" << '\n';

        // Print a separator line beneath the table headings.
        cout << string(78, '-') << '\n';

        // Run the parallel algorithms with each specified thread count.
        for (int threads : threadCounts) {

            // Tell OpenMP how many threads to use for subsequent
            // parallel regions.
            omp_set_num_threads(threads);

            // Variables to hold the maximum and minimum values
            // returned by parallelValues().
            int maxValue = 0, minValue = 0;

            // Measure the parallel value-reduction algorithm.
            const double valueTime = medianTime([&]() { parallelValues(a, maxValue, minValue); });

            // Run the function again so maxValue and minValue
            // contain the final results for the correctness check.
            parallelValues(a, maxValue, minValue);

            // Compare the parallel maximum and minimum values
            // with the serial reference values.
            const bool valuesOk = maxValue == expected.maxValue && minValue == expected.minValue;

            // Variables to hold the indices returned by the
            // critical-section algorithm.
            long long criticalMax = 0, criticalMin = 0;

            // Measure the algorithm that uses a critical section.
            //
            // The critical section is entered for each processed
            // element, which can cause considerable synchronization
            // overhead. Three samples are used to calculate the median.
            const double criticalTime = medianTime([&]() { indicesCritical(a, criticalMax, criticalMin); }, 3);

            // Run the algorithm again to obtain the final indices
            // for correctness validation.
            indicesCritical(a, criticalMax, criticalMin);

            // Verify that both indices match the serial reference,
            // including the smallest-index tie-breaking rule.
            const bool criticalOk = validIndices(a, expected, criticalMax, criticalMin);

            // Variables to hold the indices returned by the
            // manual thread-local algorithm.
            long long manualMax = 0, manualMin = 0;

            // Measure the algorithm that computes local results
            // and combines them after parallel processing.
            const double manualTime = medianTime([&]() { indicesManual(a, manualMax, manualMin); });

            // Run the algorithm again to obtain the final indices
            // for correctness validation.
            indicesManual(a, manualMax, manualMin);

            // Check the indices against the serial reference result.
            const bool manualOk = validIndices(a, expected, manualMax, manualMin);

            // Print the current thread count and the execution times
            // of all three parallel approaches.
            //
            // The final column displays PASS only when every algorithm
            // passes its corresponding correctness check.
            // Otherwise, it displays FAIL.
            cout << left << setw(9) << threads
                 << right << setw(18) << fixed << setprecision(9) << valueTime
                 << setw(20) << criticalTime
                 << setw(19) << manualTime
                 << setw(12) << ((valuesOk && criticalOk && manualOk) ? "PASS" : "FAIL") << '\n';
        }
    }

    // Print a final explanation of the correctness checks.
    // These checks verify the extreme values and ensure that when
    // duplicate extrema exist, the smallest matching index is returned.
    cout << "\nChecks verify the extrema values and exact smallest-index tie-breaking rule.\n";
}

