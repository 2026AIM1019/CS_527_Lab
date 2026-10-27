#include "../common.h"

namespace {
// Dynamic scheduling hands out batches of 100 iterations at a time.
constexpr int chunk = 100;

// Simulate uneven work: adjacent 10,000-element bands need different
// numbers of arithmetic iterations. The function is deterministic, so
// serial and parallel calculations should produce identical values.
inline double work(double x, int i) {
    const int band = (i / 10000) % 8;
    const int rounds = 12 + band * 14;
    for (int j = 0; j < rounds; ++j) {
        x = x * 1.0000001 + 0.0000001;
    }
    return x;
}
}  // namespace

void task2() {
    std::cout << "\nTask 2: OpenMP static, dynamic(" << chunk
              << "), guided scheduling\n";
    bool all = true;

    for (int n : {100000, 1000000, 10000000}) {
        std::vector<double> input(n), expected(n), result(n);
        for (int i = 0; i < n; ++i) input[i] = i * 0.001;

        // A sequential baseline provides both timing and reference output.
        const double baseline = timed([&] {
            for (int i = 0; i < n; ++i) {
                expected[i] = work(input[i], i);
            }
        });

        std::cout << "\nN=" << n << "  Serial=" << std::fixed
                  << std::setprecision(6) << baseline << " s\n";

        for (int strategy = 0; strategy < 3; ++strategy) {
            // static: predetermined chunks; dynamic: request new chunks;
            // guided: start with larger chunks and decrease chunk size.
            std::cout << (strategy == 0 ? "schedule(static)" :
                          strategy == 1 ? "schedule(dynamic,100)" :
                                          "schedule(guided)") << '\n';
            header();

            for (int threads : {1, 2, 4, 8}) {
                omp_set_dynamic(0);
                const double elapsed = timed([&] {
                    if (strategy == 0) {
#pragma omp parallel for schedule(static) num_threads(threads)
                        for (int i = 0; i < n; ++i) {
                            result[i] = work(input[i], i);
                        }
                    } else if (strategy == 1) {
#pragma omp parallel for schedule(dynamic, 100) num_threads(threads)
                        for (int i = 0; i < n; ++i) {
                            result[i] = work(input[i], i);
                        }
                    } else {
#pragma omp parallel for schedule(guided) num_threads(threads)
                        for (int i = 0; i < n; ++i) {
                            result[i] = work(input[i], i);
                        }
                    }
                });

                // Each iteration writes only result[i], avoiding races.
                const bool ok = std::equal(result.begin(), result.end(),
                                           expected.begin());
                all &= ok;
                row(threads, elapsed, baseline, ok);
            }
        }
    }

    std::cout << "Task 2 overall: " << (all ? "PASS" : "FAIL") << "\n";
}
