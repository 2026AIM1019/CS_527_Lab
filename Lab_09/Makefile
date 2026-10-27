CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic -fopenmp
SOURCES = main.cpp task1_merge_sort/main.cpp task2_scheduling/main.cpp task3_even_odd/main.cpp
TARGET = build/cs527_lab9
.PHONY: all run clean
all: $(TARGET)
$(TARGET): $(SOURCES) common.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
run: all
	./$(TARGET)
clean:
	rm -rf build
