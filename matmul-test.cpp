#include "matmul_utils.hpp"
#include <iostream>
#include <vector>
#include <cstdint>


std::vector<float> matmul_naive(const std::vector<float32>& A,
                                const std::vector<float32>& B, size_t M, size_t K, size_t N) {
    std::vector<float> C(M * N);
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < K; ++k) {
                sum += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
    return C;
}

int main() {

    int size1 = 1280;
    int size2 = 720;

    std::vector<float32> a(size1*size2);
    std::iota(a.begin(), a.end(), 0.0);

    std::vector<float32> b(size1*size2);
    std::iota(b.begin(), b.end(), 0.0);

    _rknn_matmul_ctx* ctx;

    auto start = std::chrono::high_resolution_clock::now();

    ctx = matmul_npu(size1, size2, size1, RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT32, a.data(), b.data());

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "time took: " << elapsed << " ns\n";


    for (int i = 0; i < 3; i++) {
        std::cout << "npu: " << (int64_t)((float32*)ctx->matrixC_ptr)[i] << "\n";
    }

}
