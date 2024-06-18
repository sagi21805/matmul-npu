#ifndef MATMUL_UTILS
#define MATMUL_UTILS

#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <array>

#include <cblas.h>
#include <rknpu/rknn_matmul_api.h>
#include "Float16.h"

typedef float float32;
using rknpu2::float16;

typedef _rknn_matmul_ctx;

struct _rknn_matmul_ctx {
    rknn_context ctx;
    rknn_matmul_info info;
    rknn_matmul_io_attr io_attr;
    rknn_tensor_mem* matrixA;
    rknn_tensor_mem* matrixB;
    rknn_tensor_mem* matrixC;

    void* matrixA_ptr;
    void* matrixB_ptr;
    void* matrixC_ptr;

    int32_t M;
    int32_t K;
    int32_t N;

};

_rknn_matmul_ctx* make_matmul(int32_t M, int32_t K, int32_t N);

template<typename T>
void set_matrix_data(rknn_matmul_ctx* ctx, rknn_tensor_mem* mem, rknn_matmul_tensor_attr* attr, const T* data) {

    size_t size = mem->size / sizeof(T);
    T* ptr = (T*)mem->virt_addr;
    for (size_t i = 0; i < size; ++i) {
        ptr[i] = (T)data[i];
    }
    rknn_matmul_set_io_mem(*ctx, mem, attr);
}


#endif

