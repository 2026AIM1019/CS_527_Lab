#include "../common.h"
#include <random>

namespace {
// Do not create new OpenMP tasks for ranges of at most 4096 elements.
// Sorting small ranges serially avoids excessive task-management overhead.
constexpr int cutoff = 4096;

// Merge two already-sorted, adjacent inclusive ranges: [left, mid] and
// [mid + 1, right]. tmp is a scratch array; only this range is modified.
void merge(int* a, int* tmp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    // Take the smaller current element from the left or right half.
    while (i <= mid && j <= right) {
        tmp[k++] = (a[i] <= a[j] ? a[i++] : a[j++]);
    }

    // One half may still contain elements after the other is exhausted.
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= right) tmp[k++] = a[j++];

    // Copy the merged segment back to its original positions.
    std::copy(tmp + left, tmp + right + 1, a + left);
}

// Standard recursive merge sort: split, sort both halves, then merge.
void serial(int* a, int* tmp, int left, int right) {
    if (left >= right) return;

    const int mid = left + (right - left) / 2;
    serial(a, tmp, left, mid);
    serial(a, tmp, mid + 1, right);
    merge(a, tmp, left, mid, right);
}

// Recursive task-parallel merge sort. Child tasks touch disjoint ranges,
// so they may safely share the same preallocated a and tmp arrays.
void parallel(int* a, int* tmp, int left, int right) {
    if (left >= right) return;

    // Below the cutoff, use the serial algorithm instead of spawning tasks.
    if (right - left + 1 <= cutoff) {
        serial(a, tmp, left, right);
        return;
    }

    const int mid = left + (right - left) / 2;

    // firstprivate copies range boundaries into each task's environment.
#pragma omp task shared(a, tmp) firstprivate(left, mid)
    parallel(a, tmp, left, mid);

#pragma omp task shared(a, tmp) firstprivate(mid, right)
    parallel(a, tmp, mid + 1, right);

    // Both halves MUST finish sorting before their parent merges them.
#pragma omp taskwait
    merge(a, tmp, left, mid, right);
}
}  // namespace

void task1() {
    std::cout << "\nTask 1: Recursive OpenMP task merge sort (cutoff="
              << cutoff << ")\n";
    bool all = true;

    for (int n : {100000, 1000000, 10000000}) {
        // Fixed seed makes every thread configuration sort identical data.
        std::mt19937 rng(12345);
        std::uniform_int_distribution<int> dist(-1000000, 1000000);
        std::vector<int> original(n);
        for (int& x : original) x = dist(rng);

        // Build the serial reference once per array size.
        auto reference = original;
        std::vector<int> temp(n);
        const double serial_time = timed([&] {
            serial(reference.data(), temp.data(), 0, n - 1);
        });

        std::cout << "\nN=" << n << "  Serial=" << std::fixed
                  << std::setprecision(6) << serial_time << " s\n";
        header();

        for (int threads : {1, 2, 4, 8}) {
            // Copy the original unsorted data before every timed trial.
            auto a = original;
            omp_set_dynamic(0);

            const double elapsed = timed([&] {
                // One initial thread starts the recursion; OpenMP tasks
                // then distribute its recursive branches among workers.
#pragma omp parallel num_threads(threads)
                {
#pragma omp single
                    parallel(a.data(), temp.data(), 0, n - 1);
                }
            });

            // Check both ordering and exact equality with serial output.
            const bool ok = std::is_sorted(a.begin(), a.end()) && a == reference;
            all &= ok;
            row(threads, elapsed, serial_time, ok);
        }
    }

    std::cout << "Task 1 overall: " << (all ? "PASS" : "FAIL") << "\n";
}
