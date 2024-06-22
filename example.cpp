#include "matrix_types/matrix.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {

    int rows = 4096;
    int cols = 4096;

    std::vector<int8_t> a(rows*cols, 2);

    std::vector<int8_t> b(rows*cols, 3);

    Matrix<int8_t> A(rows, cols, a.data());
    Matrix<int8_t> B(rows, cols, b.data());


    auto start = std::chrono::high_resolution_clock::now();

    Matrix<int32_t> C = A.matmul<int32_t>(B);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "Time took: " << elapsed.count() / 1.0E9f << "s\n";

    std::cout << "The first item of the matrix: ";
    std::cout << C.data[0] << "\n";
}
