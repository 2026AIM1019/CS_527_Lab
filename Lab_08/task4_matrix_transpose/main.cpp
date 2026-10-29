
// Includes the common header file, which provides commonly used libraries
// and helper functions for timing, thread counts, and result printing.
#include "../common.h"

using namespace std;

// An unnamed namespace makes the types and functions declared inside it
// accessible only within this source file.
namespace
{
    // Defines Matrix as an alias for vector<int>.
    // The matrix is stored as a one-dimensional vector instead of
    // a two-dimensional array.
    using Matrix = vector<int>;

    // Performs matrix transpose sequentially using one thread.
    // The input matrix has the specified number of rows and columns.
    // The returned matrix has 'columns' rows and 'rows' columns.
    Matrix serial_transpose(const Matrix &input, size_t rows, size_t columns)
    {
        // Create the output matrix with the same total number of elements
        // as the input matrix.
        Matrix output(rows * columns);

        // Visit every row of the input matrix.
        for (size_t row = 0; row < rows; ++row)

            // Visit every column in the current row.
            for (size_t column = 0; column < columns; ++column)

                // Copy the input element at (row, column) to the
                // transposed position (column, row).
                //
                // Row-major indexing for the input:
                // row * columns + column
                //
                // Row-major indexing for the output:
                // column * rows + row
                //
                // The number of output columns is 'rows', so the
                // output index is column * rows + row.
                output[column * rows + row] = input[row * columns + column];

        // Return the completed transposed matrix.
        return output;
    }

    // Performs matrix transpose in parallel using OpenMP.
    // The 'threads' argument specifies the requested number of threads.
    Matrix parallel_transpose(const Matrix &input, size_t rows, size_t columns, int threads)
    {
        // Create the output matrix with enough space for every element.
        // Each output element will be assigned a value during the loop.
        Matrix output(rows * columns);

        // OpenMP parallelizes the two nested loops.
        //
        // collapse(2):
        //     Combines the two nested loops into one larger iteration space,
        //     allowing OpenMP to distribute row-column pairs among threads.
        //
        // num_threads(threads):
        //     Requests the specified number of threads.
        //
        // schedule(static):
        //     Assigns chunks of loop iterations to threads in advance.
        //
        // Every (row, column) pair writes to a different output position,
        // so no lock or atomic operation is required.
#pragma omp parallel for collapse(2) num_threads(threads) schedule(static)
        // Visit each row using a signed loop counter.
        // long long is used for the loop variable and loop bound.
        for (long long row = 0; row < static_cast<long long>(rows); ++row)
        {
            // Visit each column in the current row.
            for (long long column = 0; column < static_cast<long long>(columns); ++column)
            {
                // Copy the input element at (row, column) into its
                // transposed position (column, row).
                //
                // Convert the loop counters to size_t before using them
                // in vector index calculations.
                //
                // Output index:
                // column * rows + row
                //
                // Input index:
                // row * columns + column
                output[static_cast<size_t>(column) * rows + static_cast<size_t>(row)] =
                    input[static_cast<size_t>(row) * columns + static_cast<size_t>(column)];
            }
        }

        // Return the completed output matrix.
        return output;
    }
}

// Runs the matrix transpose experiment for different matrix dimensions
// and OpenMP thread counts.
void run_task4()
{
    // Print the task title and explain the parallelization strategy.
    // collapse(2) distributes independent row-column pairs among threads.
    // Each output cell has exactly one writer.
    cout << "\nTask 4: Matrix Transpose (row-major storage)\n"
         << "collapse(2) distributes independent (row, column) pairs; each output cell has one writer.\n";

    // Test three square matrix sizes:
    // 256 x 256, 1024 x 1024, and 2048 x 2048.
    // These sizes help compare performance as the matrix grows.
    const vector<size_t> sizes{256, 1024, 2048};

    // Run the experiment separately for each matrix dimension.
    for (const size_t dimension : sizes)
    {
        // Both dimensions are set to the current size, making the matrix square.
        const size_t rows = dimension, columns = dimension;

        // Store the matrix in a one-dimensional vector.
        // The total number of elements is rows multiplied by columns.
        Matrix input(rows * columns);

        // Fill the input matrix with deterministic integer values.
        // This formula produces repeatable test data.
        for (size_t i = 0; i < input.size(); ++i)
            input[i] = static_cast<int>((i * 13U + i / 17U) % 100000U);

        // Calculate the correct transposed matrix using the serial algorithm.
        // This serves as the reference result for correctness checking.
        const Matrix reference = serial_transpose(input, rows, columns);

        // Measure the median execution time of the serial transpose.
        //
        // measure_median() is provided by common.h.
        // The lambda runs the transpose and explicitly discards its return
        // value because only the execution time is needed here.
        const double serial_time = measure_median([&]
                                                  { (void)serial_transpose(input, rows, columns); });

        // Display the current matrix dimensions and the serial timing result.
        cout << "\nMatrix size: " << rows << " x " << columns
             << "\nSerial transpose median time: " << fixed << setprecision(9) << serial_time << " s\n";

        // Print the headings for the performance comparison table.
        // This helper function is defined in common.h.
        print_timing_header();

        // Test the parallel transpose with each thread count returned
        // by thread_counts(), which is provided by common.h.
        for (const int threads : thread_counts())
        {
            // Declare a matrix to hold the parallel transpose result.
            Matrix result;

            // Measure the median execution time of the parallel transpose.
            // Each execution stores its output in result.
            const double elapsed = measure_median([&]
                                                  { result = parallel_transpose(input, rows, columns, threads); });

            // Compare the entire parallel result with the serial reference.
            // If every element matches, the result is considered correct.
            print_timing_row(threads, elapsed, serial_time, result == reference);
        }
    }
}

