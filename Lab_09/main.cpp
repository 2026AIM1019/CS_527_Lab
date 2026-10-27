#include <iostream>
#include <limits>

// Each task is implemented in its own source file.
void task1();
void task2();
void task3();

int main() {
    // Repeat the menu so the user can run several experiments in one session.
    while (true) {
        std::cout << "\n========== CS527 OpenMP Lab 9 ==========\n"
                  << "1. Task 1 - Parallel Merge Sort\n"
                  << "2. Task 2 - Scheduling Strategies\n"
                  << "3. Task 3 - Even/Odd Reductions\n"
                  << "0. Exit\n"
                  << "Choose one task to run: " << std::flush;

        int choice;
        if (!(std::cin >> choice)) {
            // EOF ends the program cleanly, including non-interactive test runs.
            if (std::cin.eof()) break;

            // Clear invalid text and allow another menu selection.
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        switch (choice) {
            case 0: return 0;
            case 1: task1(); break;
            case 2: task2(); break;
            case 3: task3(); break;
            default: std::cout << "Invalid choice.\n";
        }
    }
}
