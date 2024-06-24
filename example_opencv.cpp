#include "matrix_types/opencv_mat.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {

    int rows = 4096;
    int cols = 4096;

    std::vector<int8_t> a(rows*cols, 2);

    std::vector<int8_t> b(rows*cols, 3);

    MatNpu A(rows, cols, CV_8S, a.data());
    MatNpu B(rows, cols, CV_8S, b.data());


    auto start = std::chrono::high_resolution_clock::now();

    MatNpu C = A.matmul(B, CV_32S);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "Time took: " << elapsed.count() / 1.0E9f << "s\n";

    std::cout << "The first item of the matrix: ";
    std::cout << C.at<int32_t>(0, 0) << "\n";
}
