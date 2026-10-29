
// Includes common libraries and helper functions from the parent directory.
// These provide timing functions, thread-count utilities, and output helpers.
#include "../common.h"

// Provides mathematical functions such as fabs() for comparing floating-point values.
#include <cmath>

using namespace std;

// An unnamed namespace makes the types and functions inside it
// accessible only within this source file.
namespace
{
    // Defines Image as an alias for vector<double>.
    // The image is stored as a one-dimensional vector of pixel values.
    using Image = vector<double>;

    // Performs 2D convolution sequentially using one thread.
    // The input image is stored in row-major order.
    Image serial_convolution(const Image &image, size_t rows, size_t columns)
    {
        // Calculate the output dimensions.
        // A 3 x 3 kernel needs one input pixel on each side of a valid
        // output position, so no padding is used.
        const size_t out_rows = rows - 2, out_columns = columns - 2;

        // Create the output image and initialize every output pixel to 0.
        Image output(out_rows * out_columns, 0.0);

        // Visit every row of the output image.
        for (size_t row = 0; row < out_rows; ++row)
        {
            // Visit every column of the output image.
            for (size_t column = 0; column < out_columns; ++column)
            {
                // Store the sum of the nine input pixels covered by
                // the 3 x 3 kernel at the current output position.
                double sum = 0.0;

                // Visit the three rows of the kernel.
                for (size_t kr = 0; kr < 3; ++kr)

                    // Visit the three columns of the kernel.
                    for (size_t kc = 0; kc < 3; ++kc)

                        // Add the corresponding input pixel to the sum.
                        //
                        // (row + kr) selects the input row.
                        // (column + kc) selects the input column.
                        //
                        // Row-major indexing is:
                        // input index = row * columns + column.
                        sum += image[(row + kr) * columns + column + kc];

                // Calculate the average of the nine pixels.
                // Store it at the corresponding position in the output image.
                //
                // The output has out_columns columns, so its index is:
                // row * out_columns + column.
                output[row * out_columns + column] = sum / 9.0;
            }
        }

        // Return the completed output image.
        return output;
    }

    // Performs the same 2D convolution using OpenMP parallelization.
    // The threads argument specifies the requested number of threads.
    Image parallel_convolution(const Image &image, size_t rows, size_t columns, int threads)
    {
        // Calculate the output image dimensions for valid convolution.
        const size_t out_rows = rows - 2, out_columns = columns - 2;

        // Allocate space for all output pixels and initialize them to zero.
        Image output(out_rows * out_columns, 0.0);

        // Parallelize the two outer loops.
        //
        // collapse(2):
        //     Combines the row and column loops into one iteration space
        //     so OpenMP can distribute output positions among threads.
        //
        // num_threads(threads):
        //     Requests the specified number of threads.
        //
        // schedule(static):
        //     Assigns loop iterations to threads in predetermined chunks.
        //
        // Each iteration writes to a unique output pixel, so no critical
        // section or atomic operation is required.
#pragma omp parallel for collapse(2) num_threads(threads) schedule(static)
        // Iterate through the output rows.
        // long long is used for the loop counter.
        for (long long row = 0; row < static_cast<long long>(out_rows); ++row)
        {
            // Iterate through the output columns.
            for (long long column = 0; column < static_cast<long long>(out_columns); ++column)
            {
                // Each output pixel needs its own sum.
                // This variable belongs to the current loop iteration.
                double sum = 0.0;

                // Visit each row of the 3 x 3 kernel.
                for (size_t kr = 0; kr < 3; ++kr)
                {
                    // Visit each column of the 3 x 3 kernel.
                    for (size_t kc = 0; kc < 3; ++kc)
                    {
                        // Read the input pixel covered by the current kernel position.
                        //
                        // Convert row and column to size_t before using them
                        // in the vector index calculation.
                        // The input index is:
                        // (row + kr) * columns + column + kc.
                        sum += image[(static_cast<size_t>(row) + kr) * columns +
                                     static_cast<size_t>(column) + kc];
                    }
                }

                // Average the nine input pixels and store the result
                // in the corresponding output position.
                //
                // The output index is:
                // row * out_columns + column.
                output[static_cast<size_t>(row) * out_columns + static_cast<size_t>(column)] = sum / 9.0;
            }
        }

        // Return the output image after the parallel loop finishes.
        return output;
    }

    // Compares two images to check whether their pixel values match
    // within a small floating-point tolerance.
    bool same_image(const Image &first, const Image &second)
    {
        // Images with different numbers of pixels cannot be equal.
        if (first.size() != second.size())
            return false;

        // Compare the corresponding pixels of both images.
        for (size_t i = 0; i < first.size(); ++i)

            // fabs() returns the absolute difference between two values.
            // If the difference is greater than 1e-9, the images differ.
            if (fabs(first[i] - second[i]) > 1e-9)
                return false;

        // Every corresponding pixel matches within the tolerance.
        return true;
    }

    // Prints an image in a readable row-and-column format.
    void print_image(const Image &image, size_t rows, size_t columns)
    {
        // Visit each row of the image.
        for (size_t row = 0; row < rows; ++row)
        {
            // Visit each column in the current row.
            for (size_t column = 0; column < columns; ++column)

                // Print the pixel at the current position.
                //
                // setw(8) reserves a field width of eight characters.
                // fixed and setprecision(2) display two digits after
                // the decimal point.
                cout << setw(8) << fixed << setprecision(2) << image[row * columns + column];

            // Start a new output line after printing one complete row.
            cout << '\n';
        }
    }
}

// Runs the convolution example and performance experiments.
void run_task5()
{
    // Display the task name and explain the convolution approach.
    // Valid convolution does not add padding around the input image.
    // Each output pixel is calculated independently.
    cout << "\nTask 5: 2D Convolution (3 x 3 averaging kernel)\n"
         << "Valid convolution uses no padding; output dimensions are (rows - 2) x (columns - 2).\n"
         << "Each output pixel is independent, so the parallel loop needs no critical section.\n";

    // Define a 5 x 5 example image containing 25 pixel values.
    // The values are stored row by row in a one-dimensional vector.
    const Image example{10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150,
                        160, 170, 180, 190, 200, 210, 220, 230, 240, 250};

    // Store the expected output for the 5 x 5 example.
    // Applying a valid 3 x 3 averaging kernel produces a 3 x 3 output.
    const Image expected{70, 80, 90, 120, 130, 140, 170, 180, 190};

    // Run the example using four OpenMP threads.
    // The result should match the expected output.
    const Image example_output = parallel_convolution(example, 5, 5, 4);

    // Print the original 5 x 5 input image.
    cout << "\n5 x 5 example input:\n";

    // Display the input values in five rows and five columns.
    print_image(example, 5, 5);

    // Print the expected 3 x 3 output.
    cout << "Expected 3 x 3 output:\n";

    // Display the expected output values.
    print_image(expected, 3, 3);

    // Print the output calculated by the parallel algorithm.
    cout << "Parallel example output (4 threads):\n";

    // Display the calculated 3 x 3 output.
    print_image(example_output, 3, 3);

    // Compare the calculated example output with the expected output.
    // Print PASS if all corresponding pixel values match within tolerance.
    cout << "Example correctness: " << (same_image(example_output, expected) ? "PASS" : "FAIL") << "\n";

    // Define the image dimensions used for performance testing.
    // The program tests 128 x 128, 512 x 512, and 1024 x 1024 images.
    const vector<size_t> sizes{128, 512, 1024};

    // Run the timing experiment for each image size.
    for (const size_t dimension : sizes)
    {
        // Set both dimensions to the current size, creating a square image.
        const size_t rows = dimension, columns = dimension;

        // Allocate storage for all pixels in the input image.
        Image image(rows * columns);

        // Fill the image with deterministic values from 0 through 255.
        // This formula creates repeatable test data.
        for (size_t i = 0; i < image.size(); ++i)
            image[i] = static_cast<double>((i * 19U + i / 11U) % 256U);

        // Calculate the reference output using the serial implementation.
        // The parallel output will be compared against this result.
        const Image reference = serial_convolution(image, rows, columns);

        // Measure the median execution time of the serial convolution.
        //
        // measure_median() is provided by common.h.
        // The returned image is explicitly discarded because only
        // the execution time is needed for this measurement.
        const double serial_time = measure_median([&]
                                                  { (void)serial_convolution(image, rows, columns); });

        // Display the input and output dimensions and the serial timing.
        // The output is two rows and two columns smaller than the input.
        cout << "\nImage size: " << rows << " x " << columns << ", output: " << rows - 2 << " x " << columns - 2
             << "\nSerial convolution median time: " << fixed << setprecision(9) << serial_time << " s\n";

        // Print the headings for the performance comparison table.
        print_timing_header();

        // Run the parallel convolution using each configured thread count.
        for (const int threads : thread_counts())
        {
            // Declare a vector to hold the parallel output image.
            Image result;

            // Measure the median parallel execution time.
            // Each call calculates the convolution using the current
            // number of OpenMP threads and stores the result in result.
            const double elapsed = measure_median([&]
                                                  { result = parallel_convolution(image, rows, columns, threads); });

            // Compare the complete parallel output against the serial
            // reference, allowing a maximum per-pixel difference of 1e-9.
            // The timing helper prints PASS or FAIL along with performance.
            print_timing_row(threads, elapsed, serial_time, same_image(result, reference));
        }
    }
}
