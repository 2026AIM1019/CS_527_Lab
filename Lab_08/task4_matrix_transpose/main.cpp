#include "../common.h"

using namespace std;

namespace
{
    using Matrix = vector<int>;
    Matrix serial_transpose(const Matrix &input, size_t rows, size_t columns)
    {
        Matrix output(rows * columns);
        for (size_t row = 0; row < rows; ++row)
            for (size_t column = 0; column < columns; ++column)
                output[column * rows + row] = input[row * columns + column];
        return output;
    }
    Matrix parallel_transpose(const Matrix &input, size_t rows, size_t columns, int threads)
    {
        Matrix output(rows * columns);
#pragma omp parallel for collapse(2) num_threads(threads) schedule(static)
        for (long long row = 0; row < static_cast<long long>(rows); ++row)
        {
            for (long long column = 0; column < static_cast<long long>(columns); ++column)
            {
                output[static_cast<size_t>(column) * rows + static_cast<size_t>(row)] =
                    input[static_cast<size_t>(row) * columns + static_cast<size_t>(column)];
            }
        }
        return output;
    }
}

void run_task4()
{
    cout << "\nTask 4: Matrix Transpose (row-major storage)\n"
         << "collapse(2) distributes independent (row, column) pairs; each output cell has one writer.\n";
    const vector<size_t> sizes{256, 1024, 2048};
    for (const size_t dimension : sizes)
    {
        const size_t rows = dimension, columns = dimension;
        Matrix input(rows * columns);
        for (size_t i = 0; i < input.size(); ++i)
            input[i] = static_cast<int>((i * 13U + i / 17U) % 100000U);
        const Matrix reference = serial_transpose(input, rows, columns);
        const double serial_time = measure_median([&]
                                                  { (void)serial_transpose(input, rows, columns); });
        cout << "\nMatrix size: " << rows << " x " << columns
             << "\nSerial transpose median time: " << fixed << setprecision(9) << serial_time << " s\n";
        print_timing_header();
        for (const int threads : thread_counts())
        {
            Matrix result;
            const double elapsed = measure_median([&]
                                                  { result = parallel_transpose(input, rows, columns, threads); });
            print_timing_row(threads, elapsed, serial_time, result == reference);
        }
    }
}
