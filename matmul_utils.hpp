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
// #include "Float16.h"

typedef float float32;
typedef __fp16 float16;


typedef struct _rknn_matmul_ctx _rknn_matmul_ctx;


struct _rknn_matmul_ctx {
    rknn_context ctx;
    rknn_matmul_info info;
    rknn_matmul_io_attr io_attr;
    rknn_tensor_mem* matrixA;
    rknn_tensor_mem* matrixB;
    rknn_tensor_mem* matrixC;

    float16* matrixA_ptr;
    float16* matrixB_ptr;
    float32* matrixC_ptr;

    int32_t M;
    int32_t K;
    int32_t N;

};

_rknn_matmul_ctx* make_matmul(int32_t num_rows_a, int32_t num_cols_a, int32_t num_cols_b, _rknn_matmul_type flag) {

    _rknn_matmul_ctx* ctx = (_rknn_matmul_ctx*)malloc(sizeof(_rknn_matmul_ctx));

    memset(ctx, 0, sizeof(_rknn_matmul_ctx));

    ctx->info.M             = num_rows_a;
    ctx->info.K             = num_cols_a;
    ctx->info.N             = num_cols_b;
    ctx->info.type          = flag;
    ctx->info.AC_layout     = 0;
    ctx->info.B_layout      = 0;

    // expirmental specfic to uint8


    int ret = rknn_matmul_create(&ctx->ctx, &ctx->info, &ctx->io_attr);
    if (ret < 0) {
        printf("rknn_matmul_create fail! ret=%d\n", ret);
        abort();
    }

    // Create A
    ctx->matrixA = rknn_create_mem(ctx->ctx, ctx->io_attr.A.size);
    ctx->matrixB = rknn_create_mem(ctx->ctx, ctx->io_attr.B.size);
    ctx->matrixC = rknn_create_mem(ctx->ctx, ctx->io_attr.C.size);

    ctx->M = num_rows_a;
    ctx->K = num_cols_a;
    ctx->N = num_cols_b;

    ctx->matrixA_ptr = (float16*)ctx->matrixA->virt_addr;
    ctx->matrixB_ptr = (float16*)ctx->matrixB->virt_addr;
    ctx->matrixC_ptr = (float32*)ctx->matrixC->virt_addr;

    rknn_matmul_set_io_mem(ctx->ctx, ctx->matrixA, &ctx->io_attr.A);
    rknn_matmul_set_io_mem(ctx->ctx, ctx->matrixB, &ctx->io_attr.B);
    rknn_matmul_set_io_mem(ctx->ctx, ctx->matrixC, &ctx->io_attr.C);

    return ctx;
}


void set_matrix_data(rknn_matmul_ctx* ctx, rknn_tensor_mem* mem, rknn_matmul_tensor_attr* attr, const float* data)
{
    size_t size = mem->size / sizeof(float16);
    float16* ptr = (float16*)mem->virt_addr;
    for (size_t i = 0; i < size; ++i) {
        ptr[i] = (float16)data[i];
    }
    rknn_matmul_set_io_mem(*ctx, mem, attr);
}


// _rknn_matmul_type choose_flag(void* a, void* b, void* c) {
//     if (typeid(a) == typeid(float16) && 
//         typeid(b) == typeid(float16) && 
//         typeid(c) == typeid(float16)) {
//             return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT16;
//     } else if (
//         typeid(a) == typeid(float16) && 
//         typeid(b) == typeid(float16) && 
//         typeid(c) == typeid(float32)) {
//         return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT32;
//     } else if (
//         typeid(a) == typeid(float16) && 
//         typeid(b) == typeid(int8_t) && 
//         typeid(c) == typeid(float16)) {
//         return RKNN_FLOAT16_MM_INT8_TO_FLOAT16;
//     } else if (
//         typeid(a) == typeid(float16) && 
//         typeid(b) == typeid(int8_t) && 
//         typeid(c) == typeid(float32)) {
//         return RKNN_FLOAT16_MM_INT8_TO_FLOAT32;
//     } else if (
//         typeid(a) == typeid(int8_t) && 
//         typeid(b) == typeid(int8_t) && 
//         typeid(c) == typeid(int32_t)) {
//         return RKNN_INT8_MM_INT8_TO_INT32;
//     } else if (
//         typeid(a) == typeid(int8_t) && 
//         typeid(b) == typeid(int8_t) && 
//         typeid(c) == typeid(int8_t)) {
//         return RKNN_INT8_MM_INT8_TO_INT8;
//     } else {
//         std::cout << "unsupported combination of types:\n";
//         std::cout << "a=%s, b=%s, c=%s\n", typeid(a).name(), typeid(b).name(), typeid(c).name();
//         std::cout << "please enter types from avilable types\n";
//         std::cout << "1. float16, float16, float16\n";
//         std::cout << "2. float16, float16, float32\n";
//         std::cout << "3. float16, int8_t, float16\n";
//         std::cout << "4. float16, int8_t, float32\n";
//         std::cout << "5. int8_t, int8_t, int32_t\n";
//         std::cout << "6. int8_t, int8_t, int8_t\n";
//         abort();
//     }

// }


_rknn_matmul_ctx* matmul_npu(
    uint32_t num_rows_a,
    uint32_t num_cols_a,
    uint32_t num_cols_b,
    _rknn_matmul_type flag,
    float32* a,
    float32* b
) {

    // _rknn_matmul_type flag = choose_flag<T1, T2, T3>();
    _rknn_matmul_ctx* ctx = make_matmul(num_rows_a, num_cols_a, num_cols_b, flag);

    set_matrix_data(&ctx->ctx, ctx->matrixA, &ctx->io_attr.A, a);
    set_matrix_data(&ctx->ctx, ctx->matrixB, &ctx->io_attr.B, b);
    rknn_matmul_run(ctx->ctx);

    return ctx;

}


#endif

