CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic -fopenmp
TARGET := build/cs527_lab8
SOURCES := main.cpp task1_array_search/main.cpp task2_array_sort/main.cpp task3_histogram/main.cpp task4_matrix_transpose/main.cpp task5_convolution/main.cpp

.PHONY: all run clean
all: $(TARGET)

$(TARGET): $(SOURCES) common.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build
