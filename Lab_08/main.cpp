#include "common.h"

using namespace std;

int main() {
    omp_set_dynamic(0);
    while (true) {
        cout << "\n========== CS527 OpenMP Lab 8 ==========\n"
                  << "1. Task 1 - Array Search\n"
                  << "2. Task 2 - Array Sorting\n"
                  << "3. Task 3 - Histogram Calculation\n"
                  << "4. Task 4 - Matrix Transpose\n"
                  << "5. Task 5 - 2D Convolution\n"
                  << "0. Exit\n"
                  << "Choose one task to run: " << flush;

        int choice = -1;
        if (!(cin >> choice)) {
            cout << "\nInput ended. Exiting.\n";
            break;
        }

        if (choice == 0) {
            cout << "Exiting CS527 OpenMP Lab 8.\n";
            break;
        }

        switch (choice) {
            case 1: run_task1(); break;
            case 2: run_task2(); break;
            case 3: run_task3(); break;
            case 4: run_task4(); break;
            case 5: run_task5(); break;
            default: cout << "Invalid choice. Enter a number from 0 to 5.\n";
        }
    }
    return 0;
}
