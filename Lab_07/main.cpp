#include <iostream>

void runTask1();
void runTask2();
void runTask3();
void runTask4();
void runTask5();

int main() {
    int choice = -1;

    while (true) {
        std::cout << "\n========== CS527 OpenMP Lab ==========" << std::endl;
        std::cout << "1. Task 1 - Vector Addition" << std::endl;
        std::cout << "2. Task 2 - Sum of Array Elements" << std::endl;
        std::cout << "3. Task 3 - Maximum and Minimum" << std::endl;
        std::cout << "4. Task 4 - Matrix Addition" << std::endl;
        std::cout << "5. Task 5 - Matrix Multiplication" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choose one task to run: ";

        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                std::cout << "\nInput closed. Exiting CS527 OpenMP Lab." << std::endl;
                return 0;
            }
            std::cout << "Invalid input. Please enter a number from 0 to 5." << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                runTask1();
                break;
            case 2:
                runTask2();
                break;
            case 3:
                runTask3();
                break;
            case 4:
                runTask4();
                break;
            case 5:
                runTask5();
                break;
            case 0:
                std::cout << "Exiting CS527 OpenMP Lab." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please select a task from 1 to 5, or 0 to exit." << std::endl;
        }
    }
}
