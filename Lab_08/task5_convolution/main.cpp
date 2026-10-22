#include "../common.h"
#include <cmath>

using namespace std;

namespace
{
    using Image = vector<double>;
    Image serial_convolution(const Image &image, size_t rows, size_t columns)
    {
        const size_t out_rows = rows - 2, out_columns = columns - 2;
        Image output(out_rows * out_columns, 0.0);
        for (size_t row = 0; row < out_rows; ++row)
        {
            for (size_t column = 0; column < out_columns; ++column)
            {
                double sum = 0.0;
                for (size_t kr = 0; kr < 3; ++kr)
                    for (size_t kc = 0; kc < 3; ++kc)
                        sum += image[(row + kr) * columns + column + kc];
                output[row * out_columns + column] = sum / 9.0;
            }
        }
        return output;
    }
    Image parallel_convolution(const Image &image, size_t rows, size_t columns, int threads)
    {
        const size_t out_rows = rows - 2, out_columns = columns - 2;
        Image output(out_rows * out_columns, 0.0);
#pragma omp parallel for collapse(2) num_threads(threads) schedule(static)
        for (long long row = 0; row < static_cast<long long>(out_rows); ++row)
        {
            for (long long column = 0; column < static_cast<long long>(out_columns); ++column)
            {
                double sum = 0.0;
                for (size_t kr = 0; kr < 3; ++kr)
                {
                    for (size_t kc = 0; kc < 3; ++kc)
                    {
                        sum += image[(static_cast<size_t>(row) + kr) * columns +
                                     static_cast<size_t>(column) + kc];
                    }
                }
                output[static_cast<size_t>(row) * out_columns + static_cast<size_t>(column)] = sum / 9.0;
            }
        }
        return output;
    }
    bool same_image(const Image &first, const Image &second)
    {
        if (first.size() != second.size())
            return false;
        for (size_t i = 0; i < first.size(); ++i)
            if (fabs(first[i] - second[i]) > 1e-9)
                return false;
        return true;
    }
    void print_image(const Image &image, size_t rows, size_t columns)
    {
        for (size_t row = 0; row < rows; ++row)
        {
            for (size_t column = 0; column < columns; ++column)
                cout << setw(8) << fixed << setprecision(2) << image[row * columns + column];
            cout << '\n';
        }
    }
}

void run_task5()
{
    cout << "\nTask 5: 2D Convolution (3 x 3 averaging kernel)\n"
         << "Valid convolution uses no padding; output dimensions are (rows - 2) x (columns - 2).\n"
         << "Each output pixel is independent, so the parallel loop needs no critical section.\n";
    const Image example{10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150,
                        160, 170, 180, 190, 200, 210, 220, 230, 240, 250};
    const Image expected{70, 80, 90, 120, 130, 140, 170, 180, 190};
    
    const Image example_output = parallel_convolution(example, 5, 5, 4);
    cout << "\n5 x 5 example input:\n";
    
    print_image(example, 5, 5);
    cout << "Expected 3 x 3 output:\n";
    
    print_image(expected, 3, 3);
    cout << "Parallel example output (4 threads):\n";
    
    print_image(example_output, 3, 3);
    cout << "Example correctness: " << (same_image(example_output, expected) ? "PASS" : "FAIL") << "\n";

    const vector<size_t> sizes{128, 512, 1024};
    for (const size_t dimension : sizes)
    {
        const size_t rows = dimension, columns = dimension;
        Image image(rows * columns);

        for (size_t i = 0; i < image.size(); ++i)
            image[i] = static_cast<double>((i * 19U + i / 11U) % 256U);

        const Image reference = serial_convolution(image, rows, columns);
        const double serial_time = measure_median([&]
                                                  { (void)serial_convolution(image, rows, columns); });
        cout << "\nImage size: " << rows << " x " << columns << ", output: " << rows - 2 << " x " << columns - 2
             << "\nSerial convolution median time: " << fixed << setprecision(9) << serial_time << " s\n";
        print_timing_header();
        for (const int threads : thread_counts())
        {
            Image result;
            const double elapsed = measure_median([&]
                                                  { result = parallel_convolution(image, rows, columns, threads); });
            print_timing_row(threads, elapsed, serial_time, same_image(result, reference));
        }
    }
}
