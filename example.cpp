#include "matmul_npu.hpp"
#include <iostream>
#include <vector>

int main() {

    int rows = 32;
    int cols = 32;

    std::vector<float16> a(rows*cols, 0.2);

    std::vector<float16> b(rows*cols, 0.3);

    _matmul_ctx<float32>* ctx = matmul_npu<float32>(rows, cols, rows, a.data(), b.data());

    std::cout << "The first item of the matrix: \n";
    std::cout << ctx->result[0] << "\n";
}
