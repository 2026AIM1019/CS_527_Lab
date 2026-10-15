#include <omp.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

template <class Function>
double medianTime(Function function, int samples = 3) {
    function(); // Warm-up
    vector<double> times;
    times.reserve(samples);
    for (int sample = 0; sample < samples; ++sample) {
        const double start = omp_get_wtime();
        function();
        times.push_back(omp_get_wtime() - start);
    }
    sort(times.begin(), times.end());
    return times[times.size() / 2];
}

void multiplySerial(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k)
                sum += a[static_cast<size_t>(i) * n + k] * b[static_cast<size_t>(k) * n + j];
            c[static_cast<size_t>(i) * n + j] = sum;
        }
    }
}

void multiplyOuter(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k)
                sum += a[static_cast<size_t>(i) * n + k] * b[static_cast<size_t>(k) * n + j];
            c[static_cast<size_t>(i) * n + j] = sum;
        }
    }
}

void multiplyCollapse(const vector<double>& a, const vector<double>& b, vector<double>& c, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k)
                sum += a[static_cast<size_t>(i) * n + k] * b[static_cast<size_t>(k) * n + j];
            c[static_cast<size_t>(i) * n + j] = sum;
        }
    }
}

bool sameTask5(const vector<double>& a, const vector<double>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        const double difference = fabs(a[i] - b[i]);
        const double limit = 1e-9 * (1.0 + fabs(b[i]));
        if (difference > limit) return false;
    }
    return true;
}

void runTask5() {
    const vector<int> sizes = {256, 512, 1024};
    const vector<int> threadCounts = {1, 2, 4, 8};
    cout << "\nTask 5: Naive Matrix-Matrix Multiplication\n";
    cout << "Matrices use flattened row-major storage; C is written once per output element.\n";
    cout << "Timings are medians of 3 runs after a warm-up. N=1024 can take several minutes.\n";

    for (int n : sizes) {
        const size_t total = static_cast<size_t>(n) * n;
        cout << "\nMatrix size: " << n << " x " << n << "\n";
        vector<double> a(total), b(total), serial(total), result(total);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                a[static_cast<size_t>(i) * n + j] = static_cast<double>((i + j) % 17) / 17.0;
                b[static_cast<size_t>(i) * n + j] = static_cast<double>((i * 2 + j) % 19) / 19.0;
            }
        }

        const double serialTime = medianTime([&]() { multiplySerial(a, b, serial, n); });
        cout << "Serial median time: " << fixed << setprecision(9) << serialTime << " s\n";

        cout << left << setw(9) << "Threads"
             << right << setw(16) << "Outer (s)"
             << setw(15) << "Outer speedup"
             << setw(15) << "Outer eff."
             << setw(14) << "Outer check"
             << setw(17) << "Collapse (s)"
             << setw(18) << "Collapse speedup"
             << setw(17) << "Collapse eff."
             << setw(16) << "Collapse check" << '\n';
        cout << string(137, '-') << '\n';

        for (int threads : threadCounts) {
            omp_set_num_threads(threads);
            const double outerTime = medianTime([&]() { multiplyOuter(a, b, result, n); });
            const bool outerOk = sameTask5(serial, result);

            const double collapseTime = medianTime([&]() { multiplyCollapse(a, b, result, n); });
            const bool collapseOk = sameTask5(serial, result);

            const double outerSpeedup = outerTime > 0.0 ? serialTime / outerTime : 0.0;
            const double collapseSpeedup = collapseTime > 0.0 ? serialTime / collapseTime : 0.0;
            cout << left << setw(9) << threads
                 << right << setw(16) << fixed << setprecision(9) << outerTime
                 << setw(15) << setprecision(3) << outerSpeedup
                 << setw(15) << outerSpeedup / threads
                 << setw(14) << (outerOk ? "PASS" : "FAIL")
                 << setw(17) << fixed << setprecision(9) << collapseTime
                 << setw(18) << setprecision(3) << collapseSpeedup
                 << setw(17) << collapseSpeedup / threads
                 << setw(16) << (collapseOk ? "PASS" : "FAIL") << '\n';
        }
    }
    cout << "\nThe k loop stays sequential because its iterations contribute to the same output element.\n";
}
