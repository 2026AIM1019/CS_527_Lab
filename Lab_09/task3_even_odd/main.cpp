#include "../common.h"

namespace {
// Keep both counters together so every implementation returns the same type.
struct Counts {
    long long even = 0;
    long long odd = 0;

    bool operator==(const Counts& other) const {
        return even == other.even && odd == other.odd;
    }
};

// Serial reference: classify every integer as even or odd exactly once.
Counts serial(const std::vector<int>& a) {
    Counts counts;
    for (int value : a) {
        if (value % 2 == 0) ++counts.even;
        else ++counts.odd;
    }
    return counts;
}

// OpenMP creates private copies of both counters and combines them at the
// end. This avoids contended updates to shared counters on each element.
Counts reduction(const std::vector<int>& a, int threads) {
    long long even = 0;
    long long odd = 0;
    const long long n = static_cast<long long>(a.size());

#pragma omp parallel for reduction(+:even,odd) num_threads(threads)
    for (long long i = 0; i < n; ++i) {
        if (a[i] % 2 == 0) ++even;
        else ++odd;
    }
    return {even, odd};
}

// Optimized atomic comparison: each 4096-element block is counted locally.
// Only the two block totals require atomic additions. This is NOT the same
// as performing an atomic update for every individual input element.
Counts atomic_blocks(const std::vector<int>& a, int threads) {
    long long even = 0;
    long long odd = 0;
    const long long n = static_cast<long long>(a.size());
    const long long blocks = (n + 4095) / 4096;

#pragma omp parallel for num_threads(threads)
    for (long long b = 0; b < blocks; ++b) {
        long long local_even = 0;
        long long local_odd = 0;
        const long long end = std::min(n, (b + 1) * 4096);

        // Local counters belong to this loop iteration, so no lock is needed.
        for (long long i = b * 4096; i < end; ++i) {
            if (a[i] % 2 == 0) ++local_even;
            else ++local_odd;
        }

        // Atomics prevent lost updates when blocks finish concurrently.
#pragma omp atomic update
        even += local_even;
#pragma omp atomic update
        odd += local_odd;
    }
    return {even, odd};
}
}  // namespace

void task3() {
    std::cout << "\nTask 3: Multiple reduction vs block-aggregated atomic updates\n";
    bool all = true;

    for (int n : {100000, 1000000, 10000000, 100000000}) {
        std::vector<int> a(n);
        // Deterministic input with both odd and even values.
        for (int i = 0; i < n; ++i) {
            a[i] = (i * 17LL + 23) % 10007;
        }

        Counts expected;
        const double baseline = timed([&] { expected = serial(a); });
        std::cout << "\nN=" << n << "  Serial=" << std::fixed
                  << std::setprecision(6) << baseline << " s; even="
                  << expected.even << ", odd=" << expected.odd << "\n";

        for (int mode = 0; mode < 2; ++mode) {
            std::cout << (mode == 0 ? "reduction(+:even,odd)" :
                                      "atomic (per-block aggregated updates)")
                      << '\n';
            header();

            for (int threads : {1, 2, 4, 8}) {
                Counts actual;
                omp_set_dynamic(0);
                const double elapsed = timed([&] {
                    actual = (mode == 0) ? reduction(a, threads)
                                         : atomic_blocks(a, threads);
                });

                // The two classes must agree with serial and sum to N.
                const bool ok = actual == expected &&
                                actual.even + actual.odd == n;
                all &= ok;
                row(threads, elapsed, baseline, ok);
            }
        }
    }

    std::cout << "Task 3 overall: " << (all ? "PASS" : "FAIL") << "\n";
}
