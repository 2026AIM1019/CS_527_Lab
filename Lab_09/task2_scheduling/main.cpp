
#include "../common.h"

// Use an anonymous namespace to keep these definitions local to this
// source file. Other source files cannot directly access these names.
namespace {

// Dynamic scheduling assigns iterations to threads in batches.
// Here, each batch contains 100 loop iterations.
constexpr int chunk = 100;

// This function performs a small amount of calculation for one element.
//
// The amount of work depends on the iteration index i. This simulates a
// situation where different parts of a problem require different amounts
// of processing time.
//
// Because the function always performs the same calculations for the same
// inputs, its output remains deterministic. This allows us to compare the
// parallel results with the sequential results.
inline double work(double x, int i) {

    // Divide the input indices into bands of 10,000 elements.
    // The modulo operation cycles through eight different bands (0 to 7).
    const int band = (i / 10000) % 8;

    // Each band performs a different number of arithmetic operations.
    // Higher band values result in more iterations and therefore more work.
    const int rounds = 12 + band * 14;

    // Perform repeated arithmetic operations to simulate computational work.
    // The value of x is updated during every iteration.
    for (int j = 0; j < rounds; ++j) {
        x = x * 1.0000001 + 0.0000001;
    }

    // Return the calculated value after all arithmetic operations are complete.
    return x;
}

}  // End of the anonymous namespace.

// Task 2 compares three OpenMP loop scheduling strategies:
// 1. Static scheduling
// 2. Dynamic scheduling with a chunk size of 100
// 3. Guided scheduling
//
// For each input size, the function calculates a sequential reference result,
// measures execution times for different thread counts, and checks whether
// the parallel calculations produce the same results as the sequential ones.
void task2() {

    // Display the task name and the chunk size used for dynamic scheduling.
    std::cout << "\nTask 2: OpenMP static, dynamic(" << chunk
              << "), guided scheduling\n";

    // This variable records whether every comparison succeeds.
    // It starts as true and becomes false if any comparison fails.
    bool all = true;

    // Run the experiment using three different input sizes.
    // Increasing the input size helps us observe scheduling performance
    // as the amount of work increases.
    for (int n : {100000, 1000000, 10000000}) {

        // input:    stores the original input values.
        // expected: stores the correct results calculated sequentially.
        // result:   stores the results calculated using OpenMP.
        //
        // All three vectors contain n elements.
        std::vector<double> input(n), expected(n), result(n);

        // Initialize the input vector.
        // Each element receives its index multiplied by 0.001.
        for (int i = 0; i < n; ++i) input[i] = i * 0.001;

        // Calculate a sequential baseline.
        //
        // The timed() function measures how long the supplied code takes
        // to execute. The returned value is the elapsed execution time.
        //
        // This sequential result acts as the reference for checking the
        // correctness of all parallel scheduling strategies.
        const double baseline = timed([&] {

            // Process every element sequentially using a single loop.
            // No OpenMP parallelization is used in this baseline.
            for (int i = 0; i < n; ++i) {

                // Apply the work function to the current input element.
                // Store the result at the corresponding index in expected.
                expected[i] = work(input[i], i);
            }
        });

        // Print the input size and the sequential execution time.
        // fixed and setprecision(6) display the time with six decimal places.
        std::cout << "\nN=" << n << "  Serial=" << std::fixed
                  << std::setprecision(6) << baseline << " s\n";

        // Test all three scheduling strategies.
        //
        // strategy == 0: static scheduling
        // strategy == 1: dynamic scheduling
        // strategy == 2: guided scheduling
        for (int strategy = 0; strategy < 3; ++strategy) {

            // Display the name of the scheduling strategy currently tested.
            //
            // Static scheduling distributes iterations according to a
            // predetermined assignment.
            //
            // Dynamic scheduling assigns chunks of iterations to threads
            // as they request more work.
            //
            // Guided scheduling generally begins with larger chunks and
            // gradually reduces their size as the loop progresses.
            std::cout << (strategy == 0 ? "schedule(static)" :
                          strategy == 1 ? "schedule(dynamic,100)" :
                                          "schedule(guided)") << '\n';

            // Print the table heading used to display the timing results.
            // The definition of header() is provided elsewhere in the project.
            header();

            // Test each scheduling strategy with 1, 2, 4, and 8 threads.
            // This helps show how the number of threads affects performance.
            for (int threads : {1, 2, 4, 8}) {

                // Disable OpenMP's automatic adjustment of the number
                // of threads. The requested thread count can then be
                // supplied explicitly through num_threads(threads).
                omp_set_dynamic(0);

                // Measure the execution time of the selected parallel loop.
                // The timed() function executes the supplied code and
                // returns its elapsed execution time.
                const double elapsed = timed([&] {

                    // CASE 1: STATIC SCHEDULING
                    //
                    // OpenMP distributes loop iterations among threads
                    // using static scheduling.
                    //
                    // The assignment is predetermined rather than based
                    // on threads requesting additional chunks at runtime.
                    if (strategy == 0) {

#pragma omp parallel for schedule(static) num_threads(threads)

                        // Each thread processes its assigned iterations.
                        // Every iteration calculates one result and writes
                        // it to the corresponding position in result.
                        for (int i = 0; i < n; ++i) {
                            result[i] = work(input[i], i);
                        }

                    // CASE 2: DYNAMIC SCHEDULING
                    //
                    // Threads receive additional chunks as they become
                    // available to perform more work.
                    //
                    // The chunk size is 100 iterations. Dynamic scheduling
                    // can help when different iterations require different
                    // amounts of computation.
                    } else if (strategy == 1) {

#pragma omp parallel for schedule(dynamic, 100) num_threads(threads)

                        // Each thread processes the iterations assigned
                        // to it by the OpenMP dynamic scheduling mechanism.
                        for (int i = 0; i < n; ++i) {
                            result[i] = work(input[i], i);
                        }

                    // CASE 3: GUIDED SCHEDULING
                    //
                    // OpenMP uses guided scheduling to distribute work
                    // in chunks that generally decrease in size over time.
                    //
                    // Unlike dynamic scheduling here, no explicit chunk
                    // size is supplied, so OpenMP uses its default minimum.
                    } else {

#pragma omp parallel for schedule(guided) num_threads(threads)

                        // OpenMP distributes the iterations using guided
                        // scheduling while the threads calculate results.
                        for (int i = 0; i < n; ++i) {
                            result[i] = work(input[i], i);
                        }
                    }
                });

                // Check whether the parallel output matches the sequential
                // reference output.
                //
                // std::equal compares corresponding elements in the two
                // vectors. It returns true if all compared values are equal.
                //
                // Each loop iteration writes only to result[i]. Since
                // different iterations use different indices, threads do
                // not write to the same vector element during this loop.
                //
                // The input vector is only read, so it is not modified
                // by the parallel calculations.
                const bool ok = std::equal(result.begin(), result.end(),
                                           expected.begin());

                // Combine this comparison with the overall task status.
                //
                // If all is already false, it remains false.
                // If this comparison fails, all becomes false.
                // Therefore, all is true at the end only if every comparison
                // for every input size, strategy, and thread count succeeds.
                all &= ok;

                // Print the current thread count, parallel execution time,
                // sequential baseline, and correctness result.
                //
                // The row() function is defined elsewhere in the project.
                // It is responsible for displaying one row of the results.
                row(threads, elapsed, baseline, ok);
            }
        }
    }

    // Display the final correctness status for Task 2.
    //
    // PASS means every parallel result matched the sequential reference.
    // FAIL means at least one comparison did not match.
    std::cout << "Task 2 overall: " << (all ? "PASS" : "FAIL") << "\n";
}
