CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp
TARGET = build/cs527_openmp
SOURCES = main.cpp task1_vector_add/main.cpp task2_array_sum/main.cpp task3_max_min/main.cpp task4_matrix_add/main.cpp task5_matrix_multiply/main.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build
