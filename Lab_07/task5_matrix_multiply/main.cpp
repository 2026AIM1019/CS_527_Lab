
// Provides OpenMP functions for parallel programming.
// It is used to control the number of threads and measure execution time.
#include <omp.h>

// Provides the sort() function, which is used to sort execution times
// before calculating their median.
#include <algorithm>

// Provides mathematical functions such as fabs(), which calculates
// the absolute value of a floating-point number.
#include <cmath>

// Provides formatting tools such as fixed and setprecision()
// for displaying execution times and performance measurements.
#include <iomanip>

// Provides input and output functionality, including cout.
#include <iostream>

// Provides the vector container, which stores matrix elements
// and execution times.
#include <vector>

// Allows standard library names to be used without writing std::.
using namespace std;


// This template function measures the execution time of another function.
//
// Parameters:
//   function - The function whose execution time must be measured.
//   samples  - The number of timing samples to collect. Default is 3.
//
// Working:
//   1. Execute the function once as a warm-up.
//   2. Execute it repeatedly and record its execution time.
//   3. Sort the recorded times.
//   4. Return the median execution time.
//
// The median reduces the influence of unusually high or low timing
// measurements caused by other activities on the computer.
template <class Function>
double medianTime(Function function, int samples = 3) {

    // Execute the function once before collecting timing measurements.
    // This warm-up execution is not included in the recorded times.
    function(); // Warm-up

    // Store the execution time of each measured run.
    vector<double> times;

    // Reserve enough memory for all timing samples.
    // This helps avoid unnecessary memory allocations while recording times.
    times.reserve(samples);

    // Collect the requested number of timing samples.
    for (int sample = 0; sample < samples; ++sample) {

        // Record the starting wall-clock time in seconds.
        const double start = omp_get_wtime();

        // Execute the function being measured.
        function();

        // Calculate the elapsed time and store it in the vector.
        times.push_back(omp_get_wtime() - start);
    }

    // Sort the execution times from smallest to largest.
    sort(times.begin(), times.end());

    // Return the median execution time.
    // With three samples, this returns the middle value after sorting.
    return times[times.size() / 2];
}


// This function multiplies two square matrices using a serial algorithm.
//
// Parameters:
//   a - The first input matrix.
//   b - The second input matrix.
//   c - The output matrix that stores the multiplication result.
//   n - The number of rows and columns in each square matrix.
//
// Matrix multiplication formula:
//
//   C[i][j] = A[i][0] * B[0][j]
//           + A[i][1] * B[1][j]
//           + ...
//           + A[i][n-1] * B[n-1][j]
//
// The three nested loops perform the following operations:
//   i - Selects a row of matrix A and matrix C.
//   j - Selects a column of matrix B and matrix C.
//   k - Calculates the individual products and adds them together.
//
// This version is serial because it does not use OpenMP directives.
void multiplySerial(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {

    // Select each row of the output matrix C.
    for (int i = 0; i < n; ++i) {

        // Select each column of the output matrix C.
        for (int j = 0; j < n; ++j) {

            // Initialize the sum for the current output element.
            //
            // This variable accumulates the products of corresponding
            // elements from row i of A and column j of B.
            double sum = 0.0;

            // Calculate the dot product of row i of A and column j of B.
            //
            // Each iteration multiplies one element from A by its
            // corresponding element from B and adds the product to sum.
            for (int k = 0; k < n; ++k)

                // Access A[i][k] using its flattened one-dimensional index.
                //
                // Access B[k][j] using its flattened one-dimensional index.
                //
                // Row-major indexing formula:
                //     index = row_number * number_of_columns + column_number
                //
                // Since each matrix has n columns:
                //     A[i][k] = a[i * n + k]
                //     B[k][j] = b[k * n + j]
                //
                // Multiply the two elements and add the product to sum.
                sum += a[static_cast<size_t>(i) * n + k] * b[static_cast<size_t>(k) * n + j];

            // After the k loop finishes, sum contains the complete
            // dot product for output position C[i][j].
            //
            // Store this result in the flattened output matrix.
            //
            // Each output element is written once, after all its
            // required multiplication and addition operations are complete.
            c[static_cast<size_t>(i) * n + j] = sum;
        }
    }

    // When all three loops finish, c contains the complete matrix
    // multiplication result.
}


// This function multiplies two matrices using OpenMP parallelization
// of the outermost loop.
//
// Parameters:
//   a - The first input matrix.
//   b - The second input matrix.
//   c - The output matrix.
//   n - The number of rows and columns in each matrix.
//
// The multiplication formula and calculations are the same as in
// multiplySerial(). The difference is that the outer loop is parallel.
//
// Each thread processes one or more rows of the output matrix.
// Since different rows contain different output elements, the threads
// can calculate their assigned rows independently.
void multiplyOuter(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {

    // OpenMP distributes the outer-loop iterations among available threads.
    //
    // For example, with four threads, different threads can process
    // different rows of the output matrix.
    //
    // The inner j and k loops remain inside each assigned row.
    //
    // Each thread writes to different output elements, so no critical
    // section or reduction is required for the output matrix.
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {

        // Select each column in the current output row.
        for (int j = 0; j < n; ++j) {

            // Initialize the sum for the current output element.
            //
            // Each iteration of the j loop has its own sum variable.
            // The variable is local to the current loop execution.
            double sum = 0.0;

            // Calculate the dot product of row i of A and column j of B.
            //
            // This loop is sequential within the current output element.
            for (int k = 0; k < n; ++k)

                // Multiply the corresponding elements and accumulate
                // their product in sum.
                //
                // A[i][k] is stored at index i * n + k.
                // B[k][j] is stored at index k * n + j.
                sum += a[static_cast<size_t>(i) * n + k] * b[static_cast<size_t>(k) * n + j];

            // Store the final dot product in the output matrix.
            //
            // Each thread writes to the position associated with its
            // assigned i and j values.
            c[static_cast<size_t>(i) * n + j] = sum;
        }
    }

    // The parallel loop finishes after all assigned rows have been
    // processed. The output matrix then contains the multiplication result.
}


// This function multiplies two matrices using OpenMP's collapse clause.
//
// Parameters:
//   a - The first input matrix.
//   b - The second input matrix.
//   c - The output matrix.
//   n - The number of rows and columns.
//
// This version uses collapse(2) to combine the outer i loop and
// the middle j loop into a single logical iteration space.
//
// The k loop remains sequential because it accumulates products
// for one specific output element.
void multiplyCollapse(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {

    // Combine the two outer nested loops into one logical iteration space.
    //
    // Without collapse(2), OpenMP parallelizes only the i loop.
    //
    // With collapse(2), OpenMP can distribute the combined (i, j)
    // iterations among threads.
    //
    // There are n * n logical iterations, one for every output element.
    //
    // This can provide more scheduling flexibility because OpenMP can
    // distribute individual output-element calculations instead of
    // distributing only complete rows.
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i) {

        // Select each column of the current output row.
        for (int j = 0; j < n; ++j) {

            // Initialize the accumulated sum for output element C[i][j].
            double sum = 0.0;

            // Calculate the dot product for the current output element.
            //
            // The k loop remains sequential. All its iterations
            // contribute to the same sum variable.
            for (int k = 0; k < n; ++k)

                // Multiply A[i][k] by B[k][j] and add the result
                // to the running sum.
                //
                // Both matrices use flattened row-major storage.
                sum += a[static_cast<size_t>(i) * n + k] * b[static_cast<size_t>(k) * n + j];

            // Store the completed dot product in C[i][j].
            //
            // Each logical (i, j) iteration writes to its own output
            // position, avoiding conflicting writes between threads.
            c[static_cast<size_t>(i) * n + j] = sum;
        }
    }

    // After the collapsed parallel loop completes, the output matrix
    // contains the multiplication result.
}


// This function compares two matrix results to check whether they
// are approximately equal.
//
// Parameters:
//   a - The first result vector.
//   b - The second result vector, used as the reference for the
//       element-wise tolerance calculation.
//
// Return:
//   true  - If the vectors have the same size and every corresponding
//           element satisfies the tolerance condition.
//   false - If their sizes differ or any element fails the comparison.
//
// Floating-point calculations may produce small rounding differences.
// Therefore, the function uses a tolerance rather than requiring
// exact equality between double values.
bool sameTask5(const vector<double>& a, const vector<double>& b) {

    // The vectors must have the same number of elements.
    // Different sizes mean the results cannot match element by element.
    if (a.size() != b.size()) return false;

    // Compare the corresponding elements of the two result vectors.
    for (size_t i = 0; i < a.size(); ++i) {

        // Calculate the absolute difference between the two elements.
        //
        // fabs() returns the non-negative magnitude of the difference.
        const double difference = fabs(a[i] - b[i]);

        // Calculate the allowed difference for the current element.
        //
        // The tolerance is relative to the magnitude of b[i].
        //
        // When b[i] is close to zero, the tolerance is approximately
        // 1e-9.
        //
        // For larger values, the allowed difference increases with
        // the magnitude of the reference value.
        const double limit = 1e-9 * (1.0 + fabs(b[i]));

        // If the difference exceeds the allowed limit, the results
        // are considered different.
        if (difference > limit) return false;
    }

    // Every element passed the tolerance check, so the two vectors
    // are considered approximately equal.
    return true;
}


// This function runs Task 5, which compares different approaches
// for performing square matrix-matrix multiplication.
//
// The program compares:
//   1. Serial matrix multiplication.
//   2. Parallel multiplication using the outer loop.
//   3. Parallel multiplication using collapse(2).
//
// For each matrix size, it:
//   - Generates input matrices.
//   - Calculates a serial reference result.
//   - Measures serial execution time.
//   - Runs the parallel approaches with different thread counts.
//   - Calculates speedup and parallel efficiency.
//   - Checks the correctness of each parallel result.
//   - Prints a performance comparison table.
void runTask5() {

    // Define the matrix dimensions to be tested.
    //
    // The program tests 256 x 256, 512 x 512, and 1024 x 1024 matrices.
    //
    // Matrix multiplication requires approximately 2 * n^3 arithmetic
    // operations under the conventional counting of a multiplication
    // and an addition as separate operations.
    //
    // Consequently, doubling n increases the computational work
    // by approximately eight times.
    const vector<int> sizes = {256, 512, 1024};

    // Define the OpenMP thread counts used in the experiment.
    //
    // Each matrix size is tested with 1, 2, 4, and 8 threads.
    const vector<int> threadCounts = {1, 2, 4, 8};

    // Print the task heading.
    cout << "\nTask 5: Naive Matrix-Matrix Multiplication\n";

    // Explain that matrices are stored in flattened row-major order
    // and that each output element is written once.
    cout << "Matrices use flattened row-major storage; C is written once per output element.\n";

    // Explain the timing methodology and warn that the largest matrix
    // can take considerably longer to process.
    cout << "Timings are medians of 3 runs after a warm-up. N=1024 can take several minutes.\n";

    // Perform the complete experiment separately for every matrix size.
    for (int n : sizes) {

        // Calculate the total number of elements in an n x n matrix.
        //
        // For example:
        //   n = 256  -> 65,536 elements.
        //   n = 512  -> 262,144 elements.
        //   n = 1024 -> 1,048,576 elements.
        const size_t total = static_cast<size_t>(n) * n;

        // Print the dimensions of the current matrices.
        cout << "\nMatrix size: " << n << " x " << n << "\n";

        // Allocate memory for the two input matrices and two result vectors.
        //
        // a      - First input matrix.
        // b      - Second input matrix.
        // serial - Stores the serial multiplication result.
        // result - Stores the result of each parallel implementation.
        //
        // All vectors contain 'total' double elements.
        vector<double> a(total), b(total), serial(total), result(total);

        // Initialize the input matrices.
        //
        // The two nested loops visit every row and column.
        // The formulas generate deterministic floating-point values.
        //
        // The same matrix size therefore produces the same input values
        // each time the program is run.
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {

                // Initialize element A[i][j].
                //
                // The expression (i + j) % 17 generates a remainder
                // from 0 to 16. Dividing by 17.0 converts the result
                // into a double value between 0 and 16/17.
                //
                // The element is stored at flattened index i * n + j.
                a[static_cast<size_t>(i) * n + j] = static_cast<double>((i + j) % 17) / 17.0;

                // Initialize element B[i][j].
                //
                // The expression (i * 2 + j) % 19 generates a remainder
                // from 0 to 18. Dividing by 19.0 produces a double value
                // between 0 and 18/19.
                //
                // The element is stored at flattened index i * n + j.
                b[static_cast<size_t>(i) * n + j] = static_cast<double>((i * 2 + j) % 19) / 19.0;
            }
        }

        // Measure the serial matrix multiplication time.
        //
        // The lambda function calls multiplySerial() using the two
        // input matrices and stores the result in the serial vector.
        //
        // medianTime() performs one warm-up execution and then collects
        // three measured samples.
        //
        // The returned value is the median execution time in seconds.
        const double serialTime = medianTime([&]() { multiplySerial(a, b, serial, n); });

        // Print the median serial execution time.
        //
        // fixed and setprecision(9) display nine digits after
        // the decimal point.
        cout << "Serial median time: " << fixed << setprecision(9) << serialTime << " s\n";

        // Print the headings for the performance comparison table.
        //
        // Threads:
        //   Number of OpenMP threads.
        //
        // Outer (s):
        //   Median time for outer-loop parallelization.
        //
        // Outer speedup:
        //   Serial execution time divided by outer-loop execution time.
        //
        // Outer eff.:
        //   Outer-loop speedup divided by the number of threads.
        //
        // Outer check:
        //   Whether the outer-loop result matches the serial result.
        //
        // Collapse (s):
        //   Median time for collapse(2) parallelization.
        //
        // Collapse speedup:
        //   Serial execution time divided by collapse execution time.
        //
        // Collapse eff.:
        //   Collapse speedup divided by the number of threads.
        //
        // Collapse check:
        //   Whether the collapse result matches the serial result.
        cout << left << setw(9) << "Threads"
             << right << setw(16) << "Outer (s)"
             << setw(15) << "Outer speedup"
             << setw(15) << "Outer eff."
             << setw(14) << "Outer check"
             << setw(17) << "Collapse (s)"
             << setw(18) << "Collapse speedup"
             << setw(17) << "Collapse eff."
             << setw(16) << "Collapse check" << '\n';

        // Print a separator beneath the column headings.
        cout << string(137, '-') << '\n';

        // Repeat the parallel experiments with the selected thread counts.
        for (int threads : threadCounts) {

            // Configure OpenMP to use the current number of threads
            // for subsequent parallel regions.
            omp_set_num_threads(threads);

            // Measure the outer-loop parallel multiplication.
            //
            // multiplyOuter() calculates the matrix product and stores
            // it in result.
            //
            // medianTime() performs one warm-up execution and then
            // measures three additional executions.
            const double outerTime = medianTime([&]() { multiplyOuter(a, b, result, n); });

            // Check whether the result from multiplyOuter() agrees
            // with the serial reference matrix.
            //
            // The comparison uses the relative tolerance implemented
            // in sameTask5().
            const bool outerOk = sameTask5(serial, result);

            // Measure the multiplication approach using collapse(2).
            //
            // This function overwrites the result vector with the
            // output of the collapsed-loop implementation.
            const double collapseTime = medianTime([&]() { multiplyCollapse(a, b, result, n); });

            // Compare the collapse(2) output with the serial reference.
            //
            // This verifies that the second parallel implementation
            // produces approximately the same matrix.
            const bool collapseOk = sameTask5(serial, result);

            // Calculate the speedup of the outer-loop parallel approach.
            //
            // Speedup = Serial execution time / Parallel execution time
            //
            // A speedup greater than 1 indicates that the measured
            // parallel execution was faster than the serial execution.
            //
            // The conditional operator avoids division by zero:
            // if outerTime is not positive, speedup is set to 0.
            const double outerSpeedup = outerTime > 0.0 ? serialTime / outerTime : 0.0;

            // Calculate the speedup of the collapse(2) approach.
            //
            // The calculation follows the same formula as outerSpeedup.
            // If collapseTime is not positive, the result is set to 0.
            const double collapseSpeedup = collapseTime > 0.0 ? serialTime / collapseTime : 0.0;

            // Print the performance results for the current thread count.
            //
            // left and right control the alignment of table columns.
            // setw() sets the minimum width of each column.
            //
            // fixed and setprecision(9) display execution times with
            // nine decimal places.
            //
            // setprecision(3) displays speedup and efficiency values
            // with three digits after the decimal point.
            //
            // The conditional operator prints PASS if the relevant
            // correctness check succeeds and FAIL otherwise.
            cout << left << setw(9) << threads
                 << right << setw(16) << fixed << setprecision(9) << outerTime
                 << setw(15) << setprecision(3) << outerSpeedup

                 // Parallel efficiency = Speedup / Number of threads.
                 //
                 // An efficiency near 1.0 represents speedup close
                 // to the ideal linear speedup for that thread count.
                 // Real-world performance is affected by overhead,
                 // memory access, and other hardware limitations.
                 << setw(15) << outerSpeedup / threads

                 << setw(14) << (outerOk ? "PASS" : "FAIL")
                 << setw(17) << fixed << setprecision(9) << collapseTime
                 << setw(18) << setprecision(3) << collapseSpeedup

                 // Calculate the parallel efficiency of collapse(2).
                 << setw(17) << collapseSpeedup / threads

                 << setw(16) << (collapseOk ? "PASS" : "FAIL") << '\n';
        }

        // After all thread counts have been tested, the outer loop
        // moves to the next matrix size.
    }

    // Print the explanation of why the innermost k loop remains sequential.
    //
    // For a fixed pair (i, j), every iteration of k contributes
    // to the same output element C[i][j] through the sum variable.
    //
    // The existing implementation calculates that sum sequentially.
    // Parallelizing k would require a different accumulation strategy,
    // such as a reduction, to combine partial sums correctly.
    cout << "\nThe k loop stays sequential because its iterations contribute to the same output element.\n";
}
