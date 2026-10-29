
// Provides OpenMP functions for parallel programming.
// It allows us to create parallel loops, control the number of threads,
// and measure execution time using omp_get_wtime().
#include <omp.h>

// Provides the sort() function, which is used to sort execution times
// before calculating their median.
#include <algorithm>

// Provides mathematical functions such as fabs(), which calculates
// the absolute value of a floating-point number.
#include <cmath>

// Provides function-related utilities from the C++ standard library.
#include <functional>

// Provides formatting tools such as fixed and setprecision()
// for displaying execution times in a readable format.
#include <iomanip>

// Provides input and output functionality, including cout.
#include <iostream>

// Provides the string class, which is used when printing separator lines.
#include <string>

// Provides the vector container, which stores matrix elements
// and the execution times collected during benchmarking.
#include <vector>

// Allows us to use standard library names without writing std::.
using namespace std;


// This template function measures the execution time of another function.
//
// Parameters:
//   function     - The function whose execution time is measured.
//   samples      - The number of timing samples to collect. Default is 3.
//   repetitions  - The number of times the function runs within each sample.
//                  Default is 1.
//
// Working:
//   1. Execute the function once as a warm-up.
//   2. Collect multiple timing samples.
//   3. Execute the function 'repetitions' times in each sample.
//   4. Calculate the average time per call for each sample.
//   5. Sort the sample times.
//   6. Return the median time per call.
//
// Measuring the median helps reduce the influence of unusually slow
// or fast measurements caused by other system activities.
template <class Function>
double medianTime(Function function, int samples = 3, int repetitions = 1) {

    // Execute the function once before collecting timing measurements.
    // This warm-up run is not included in the recorded execution times.
    function(); // Warm-up

    // Store the measured time for each sample.
    vector<double> times;

    // Reserve enough memory for all timing samples.
    // This helps avoid unnecessary memory allocations while recording times.
    times.reserve(samples);

    // Collect the requested number of timing samples.
    for (int sample = 0; sample < samples; ++sample) {

        // Record the starting wall-clock time.
        // omp_get_wtime() returns the current time in seconds.
        const double start = omp_get_wtime();

        // Execute the function the specified number of times.
        //
        // Repetitions are useful for very small operations because
        // a single call might take too little time to measure accurately.
        for (int rep = 0; rep < repetitions; ++rep) function();

        // Calculate the total time spent on this sample.
        //
        // Divide by repetitions to obtain the average execution time
        // per function call during this sample.
        //
        // Store this average in the times vector.
        times.push_back((omp_get_wtime() - start) / repetitions);
    }

    // Sort the measured times in ascending order.
    sort(times.begin(), times.end());

    // Return the median execution time.
    //
    // For three samples, this returns the second value after sorting.
    // The median is less sensitive to an unusually high or low sample
    // than the arithmetic mean.
    return times[times.size() / 2];
}


// This function adds two matrices using a serial approach.
//
// Parameters:
//   a - The first input matrix.
//   b - The second input matrix.
//   c - The output matrix that stores the sum.
//   n - The number of rows and columns in each square matrix.
//
// All three matrices are stored as one-dimensional vectors.
// A matrix with n rows and n columns contains n * n elements.
//
// The function uses two nested loops:
//   - The outer loop selects a row.
//   - The inner loop selects a column.
//
// Since there is no OpenMP directive, the loop iterations are
// executed sequentially by a single thread.
void addSerial(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {

    // Traverse the matrix one row at a time.
    for (int i = 0; i < n; ++i)

        // Traverse every column in the current row.
        for (int j = 0; j < n; ++j) {

            // Convert the two-dimensional matrix position (i, j)
            // into an index in the one-dimensional vector.
            //
            // In row-major order, each complete row is stored
            // consecutively in memory.
            //
            // The formula is:
            //     index = row_number * number_of_columns + column_number
            //
            // Here, every row contains n elements.
            //
            // static_cast<size_t>(i) converts i to size_t before
            // multiplication, which is commonly used for container sizes
            // and indices.
            const size_t index = static_cast<size_t>(i) * n + j;

            // Add the corresponding elements from matrices a and b.
            // Store the result in the same position in matrix c.
            //
            // Mathematically:
            //     C[i][j] = A[i][j] + B[i][j]
            c[index] = a[index] + b[index];
        }

    // After both loops finish, c contains the complete matrix sum.
}


// This function performs matrix addition using OpenMP parallelization
// of the outer loop.
//
// Parameters:
//   a - The first input matrix.
//   b - The second input matrix.
//   c - The output matrix.
//   n - The number of rows and columns.
//
// Unlike addSerial(), this function distributes the outer-loop
// iterations among multiple OpenMP threads.
//
// Each outer-loop iteration processes one complete row.
// Since different rows contain different output elements,
// threads can calculate their assigned rows independently.
void addOuter(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {

    // OpenMP parallel for distributes the iterations of the outer loop
    // among the available threads.
    //
    // For example, with four threads, OpenMP may assign different
    // groups of rows to different threads.
    //
    // Each thread executes the inner loop for its assigned rows.
    //
    // No critical section or reduction is required because each
    // iteration writes to a different section of the output vector.
    #pragma omp parallel for
    for (int i = 0; i < n; ++i)

        // For the current row, process every column.
        for (int j = 0; j < n; ++j) {

            // Calculate the one-dimensional index corresponding
            // to the current row and column.
            //
            // The matrix is stored in row-major order.
            const size_t index = static_cast<size_t>(i) * n + j;

            // Add the elements at the current position and store
            // the result in the output matrix.
            c[index] = a[index] + b[index];
        }

    // When the parallel loop completes, all matrix elements
    // in the output vector have been calculated.
}


// This function performs matrix addition using OpenMP's collapse clause.
//
// Parameters:
//   a - The first input matrix.
//   b - The second input matrix.
//   c - The output matrix.
//   n - The number of rows and columns.
//
// The main difference between this function and addOuter() is
// the use of collapse(2).
//
// addOuter() parallelizes the outer loop, while collapse(2) combines
// the two nested loops into a single logical iteration space.
// This allows OpenMP to distribute iterations across both dimensions.
void addCollapse(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {

    // The collapse(2) clause tells OpenMP to combine the two
    // perfectly nested loops into one logical iteration space.
    //
    // The combined loop contains n * n logical iterations,
    // one for each matrix element.
    //
    // This can give OpenMP more iterations to distribute among threads,
    // especially when the outer loop alone provides relatively few
    // iterations.
    //
    // Each logical iteration writes to a distinct output element.
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i)

        // Iterate through the columns of the current row.
        for (int j = 0; j < n; ++j) {

            // Convert the two-dimensional matrix position (i, j)
            // into a one-dimensional index.
            const size_t index = static_cast<size_t>(i) * n + j;

            // Add the two corresponding matrix elements.
            // Store their sum in the output matrix.
            c[index] = a[index] + b[index];
        }

    // Once the parallel loop finishes, c contains the matrix sum.
}


// This function checks whether two vectors contain the same values
// within a specified floating-point tolerance.
//
// Parameters:
//   a - The first vector.
//   b - The second vector.
//
// Return:
//   true  - If the vectors have the same size and all corresponding
//           elements differ by no more than the allowed tolerance.
//   false - If their sizes differ or any element fails the comparison.
//
// This function is used to compare the serial matrix-addition result
// with the results produced by the parallel implementations.
bool sameTask4(const vector<double>& a, const vector<double>& b) {

    // The vectors must have the same number of elements.
    // If their sizes differ, they cannot represent identical results.
    if (a.size() != b.size()) return false;

    // Examine every corresponding element in the two vectors.
    for (size_t i = 0; i < a.size(); ++i)

        // Calculate the absolute difference between the two values.
        //
        // fabs() is used because the elements are double values.
        //
        // If the absolute difference is greater than 1e-12,
        // the values are considered different.
        //
        // The tolerance is 0.000000000001.
        // A small tolerance is commonly used when comparing
        // floating-point values.
        if (fabs(a[i] - b[i]) > 1e-12) return false;

    // If no element fails the comparison, the vectors are considered
    // equal within the specified tolerance.
    return true;
}


// This function runs Task 4, which compares different approaches
// for performing matrix addition.
//
// The experiment compares:
//   1. Serial matrix addition.
//   2. Parallel matrix addition using the outer loop.
//   3. Parallel matrix addition using collapse(2).
//
// It tests multiple matrix sizes and thread counts.
//
// For each configuration, it measures execution time and checks
// whether the parallel result matches the serial result.
void runTask4() {

    // Define the matrix sizes used in the experiment.
    //
    // 8 means an 8 x 8 matrix containing 64 elements.
    //
    // 4096 means a 4096 x 4096 matrix containing
    // 16,777,216 elements.
    //
    // Testing both sizes allows the program to compare performance
    // on small matrices and much larger matrices.
    const vector<int> sizes = {8, 4096};

    // Define the number of OpenMP threads used for testing.
    //
    // Each matrix size is tested with 1, 2, 4, and 8 threads.
    const vector<int> threadCounts = {1, 2, 4, 8};

    // Print the heading for Task 4.
    // The matrices are represented as flattened one-dimensional vectors
    // using row-major storage.
    cout << "\nTask 4: Matrix Addition (flattened row-major matrices)\n";

    // Explain how the execution times are calculated.
    cout << "Timings are median per-call times from 3 samples after a warm-up.\n";

    // Explain why multiple calls are used for the smallest matrix.
    // A single addition of two 8 x 8 matrices can finish very quickly,
    // making its execution time difficult to measure accurately.
    cout << "For N=8, each sample batches 200 calls to reduce timer-resolution noise.\n";

    // Run the experiment separately for each matrix size.
    for (int n : sizes) {

        // Calculate the total number of elements in the square matrix.
        //
        // Convert n to size_t before multiplication so that the
        // multiplication is performed using an unsigned size type.
        const size_t total = static_cast<size_t>(n) * n;

        // Display the dimensions of the current matrix.
        cout << "\nMatrix size: " << n << " x " << n << "\n";

        // Create the two input matrices and two output vectors.
        //
        // a      - First input matrix.
        // b      - Second input matrix.
        // serial - Stores the result from the serial algorithm.
        // result - Stores the result from a parallel algorithm.
        //
        // Each vector is initialized with 'total' elements.
        vector<double> a(total), b(total), serial(total), result(total);

        // Initialize the input matrices with deterministic values.
        //
        // The values are generated using the element index.
        // The modulo operations keep the values within bounded ranges.
        //
        // Matrix a:
        //   Uses values from (i % 101) multiplied by 0.5.
        //
        // Matrix b:
        //   Uses values from (i % 79) multiplied by 0.25.
        //
        // Since the formulas are deterministic, repeating the
        // experiment produces the same input values for the same size.
        for (size_t i = 0; i < total; ++i) {
            a[i] = static_cast<double>(i % 101) * 0.5;
            b[i] = static_cast<double>(i % 79) * 0.25;
        }

        // Choose how many times to repeat each matrix addition
        // during a single timing sample.
        //
        // For the 8 x 8 matrix, perform 200 calls per sample because
        // each call is very short.
        //
        // For the larger matrix, use one call per sample because
        // processing millions of elements already takes more time.
        const int repetitions = n == 8 ? 200 : 1;

        // Measure the serial matrix-addition execution time.
        //
        // The lambda function calls addSerial() with the two input
        // matrices, the serial output vector, and the matrix dimension.
        //
        // Three timing samples are collected.
        // Each sample executes the function 'repetitions' times.
        // medianTime() returns the median per-call time.
        const double serialTime = medianTime([&]() { addSerial(a, b, serial, n); }, 3, repetitions);

        // Display the median serial execution time in seconds.
        //
        // fixed and setprecision(9) show nine digits after the
        // decimal point.
        cout << "Serial median time: " << fixed << setprecision(9) << serialTime << " s\n";

        // Print the headings for the performance comparison table.
        //
        // Threads:
        //   Number of OpenMP threads.
        //
        // Outer (s):
        //   Median execution time for outer-loop parallelization.
        //
        // Outer check:
        //   Whether the outer-loop result matches the serial result.
        //
        // Collapse (s):
        //   Median execution time using collapse(2).
        //
        // Collapse check:
        //   Whether the collapse result matches the serial result.
        cout << left << setw(9) << "Threads"
             << right << setw(18) << "Outer (s)"
             << setw(16) << "Outer check"
             << setw(18) << "Collapse (s)"
             << setw(18) << "Collapse check" << '\n';

        // Print a horizontal separator beneath the table headings.
        cout << string(79, '-') << '\n';

        // Repeat the parallel experiments using each thread count.
        for (int threads : threadCounts) {

            // Set the number of OpenMP threads for the parallel
            // regions executed by the following functions.
            omp_set_num_threads(threads);

            // Measure the execution time of outer-loop parallelization.
            //
            // The lambda function calls addOuter().
            // The resulting matrix is stored in 'result'.
            //
            // Three timing samples are collected, with each sample
            // executing the function 'repetitions' times.
            const double outerTime = medianTime([&]() { addOuter(a, b, result, n); }, 3, repetitions);

            // Compare the outer-loop result with the serial result.
            //
            // The serial vector contains the reference answer.
            // The result vector contains the latest output from addOuter().
            //
            // PASS means all corresponding elements match within
            // the tolerance used by sameTask4().
            const bool outerOk = sameTask4(serial, result);

            // Measure the execution time of the collapse(2) approach.
            //
            // The output is stored in the same result vector.
            // Each call overwrites the previous contents with the
            // newly calculated matrix sum.
            const double collapseTime = medianTime([&]() { addCollapse(a, b, result, n); }, 3, repetitions);

            // Compare the collapse(2) result with the serial reference.
            //
            // This verifies the output produced by the second
            // parallel implementation.
            const bool collapseOk = sameTask4(serial, result);

            // Print the current thread count, the execution times,
            // and the correctness checks for both parallel approaches.
            //
            // left aligns the thread-count column.
            // right aligns the remaining columns.
            // setw() controls the minimum width of each column.
            //
            // The conditional operator prints:
            //   PASS - If the corresponding correctness check succeeds.
            //   FAIL - If the corresponding correctness check fails.
            cout << left << setw(9) << threads
                 << right << setw(18) << fixed << setprecision(9) << outerTime
                 << setw(16) << (outerOk ? "PASS" : "FAIL")
                 << setw(18) << collapseTime
                 << setw(18) << (collapseOk ? "PASS" : "FAIL") << '\n';
        }

        // After all thread counts have been tested, the program
        // proceeds to the next matrix size, if one remains.
    }

    // After both matrix sizes have been processed, runTask4() ends.
}

