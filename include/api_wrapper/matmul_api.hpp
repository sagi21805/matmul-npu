#ifndef MATMUL_NPU
#define MATMUL_NPU

#include <rknpu/rknn_matmul_api.h>
#include <type_traits>
#include <iostream>
#include <cstring>
#include "utils/half.hpp"

using float16 = half_float::half;
typedef float float32;

/**
 * @brief Utility function to choose flag from the _rknn_matmul_types
 * 
 * @param To    The type of the output matrix
 * @param Ti1   The type of the first input martix
 * @param Ti2   The type of the second input matrix
 * 
 * @return _rknn_matmul_ type flag for the matmul operation
 */
template<typename To, typename Ti1, typename Ti2>
_rknn_matmul_type choose_matmul_type() {
    if (
        std::is_same<float16, Ti1>::value && 
        std::is_same<float16, Ti2>::value && 
        std::is_same<float16, To>::value) {
        return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT16;
    } else if (
        std::is_same<float16, Ti1>::value && 
        std::is_same<float16, Ti2>::value && 
        std::is_same<float32, To>::value)  {
        return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT32;
    } else if (
        std::is_same<float16, Ti1>::value &&
        std::is_same<int8_t, Ti2>::value  &&
        std::is_same<float16, To>::value ) { 
        return RKNN_FLOAT16_MM_INT8_TO_FLOAT16; 
    } else if (
        std::is_same<int8_t, Ti1>::value && 
        std::is_same<int8_t, Ti2>::value && 
        std::is_same<int8_t, To>::value)  {
        return RKNN_INT8_MM_INT8_TO_INT8;
    } else if (
        std::is_same<int8_t, Ti1>::value && 
        std::is_same<int8_t, Ti2>::value && 
        std::is_same<int32_t, To>::value)  {
        return RKNN_INT8_MM_INT8_TO_INT32;
    } else {
        std::cout << "unsupported combination of types:\n";
        std::cout << "please enter types from avilable types\n";
        std::cout << "1. float16, float16, float16\n";
        std::cout << "2. float16, float16, float32\n";
        std::cout << "3. float16, int8_t, float16\n";
        std::cout << "4. int8_t, int8_t, int8_t\n";
        std::cout << "5. int8_t, int8_t, int32_t\n";
        abort();
    }

}

/**
 * Struct that wraps all the built in rknn types 
 * and contains the result pointer
 * 
 * @param To The type of the output matrix
 */
struct _matmul_ctx {
    rknn_context ctx;
    rknn_matmul_info info;
    rknn_matmul_io_attr io_attr;
    rknn_tensor_mem* matrixA;
    rknn_tensor_mem* matrixB;
    rknn_tensor_mem* matrixC;
};

/**
 * Struct that contains the result tensor of the matmul and it's context
 */
struct tensor_result {
    rknn_context ctx;
    rknn_tensor_mem* resultMatrix;

    tensor_result(rknn_context ctx, rknn_tensor_mem* resultMatrix) 
        : ctx(ctx), resultMatrix(resultMatrix) {}
};

/**
 * @brief ## __Create a matmul operation for the npu__
 * 
 * @param To The type of the output matrix
 * 
 * @return _matmul_ctx with the currect context for the rknn_matmul_run function
 */
_matmul_ctx* make_matmul(
    int32_t num_rows_a, int32_t num_cols_a, int32_t num_cols_b, _rknn_matmul_type type
    ) {

    /* create a matmul_ctx struct */
    _matmul_ctx* matmul_ctx = (_matmul_ctx*)malloc(sizeof(_matmul_ctx));

    /* set all field to zero */
    memset(matmul_ctx, 0, sizeof(_matmul_ctx));

    matmul_ctx->info.M             = num_rows_a; /* set first matrix rows */
    matmul_ctx->info.K             = num_cols_a; /* set first matrix cols */
    matmul_ctx->info.N             = num_cols_b; /* set second matrix cols */
    matmul_ctx->info.type          = type; /* set the dtypes of the input and output matrices*/
    matmul_ctx->info.AC_layout     = 0; /* set the layout of matrices A and C */
    matmul_ctx->info.B_layout      = 0; /* set the layout of matrix B */
    

    // create the matmul operation
    int ret = rknn_matmul_create(&matmul_ctx->ctx, &matmul_ctx->info, &matmul_ctx->io_attr);
    if (ret < 0) {
        printf("rknn_matmul_create fail! ret=%d\n", ret);
        abort();
    }

    // create the memory for the matrices in the npu
    matmul_ctx->matrixA = rknn_create_mem(matmul_ctx->ctx, matmul_ctx->io_attr.A.size);
    matmul_ctx->matrixB = rknn_create_mem(matmul_ctx->ctx, matmul_ctx->io_attr.B.size);
    matmul_ctx->matrixC = rknn_create_mem(matmul_ctx->ctx, matmul_ctx->io_attr.C.size);


    // set the memory in the npu
    rknn_matmul_set_io_mem(matmul_ctx->ctx, matmul_ctx->matrixA, &matmul_ctx->io_attr.A);
    rknn_matmul_set_io_mem(matmul_ctx->ctx, matmul_ctx->matrixB, &matmul_ctx->io_attr.B);
    rknn_matmul_set_io_mem(matmul_ctx->ctx, matmul_ctx->matrixC, &matmul_ctx->io_attr.C);

    return matmul_ctx;
}

/**
 * @brief Set the matrix data in the npu
 * 
 * @param Ti The type of the input matrix
 * @param ctx The context for the matmul operation
 * @param mem The information of the matrix tensor memory
 * @param attr The attributes of the matrix tensor
 */
void set_matrix_data(
    rknn_matmul_ctx* ctx, 
    rknn_tensor_mem* mem, 
    rknn_matmul_tensor_attr* attr, 
    const void* data ) {

    memcpy(mem->virt_addr, data, mem->size);
    rknn_matmul_set_io_mem(*ctx, mem, attr);
}

/**
 * @brief Free the matrices tensors 
 * 
 * @param To The type of the input matrix
 * @param ctx The context of the matmul operation
 */
void free_matmul(_matmul_ctx* ctx) {
    rknn_destroy_mem(ctx->ctx, ctx->matrixA);
    rknn_destroy_mem(ctx->ctx, ctx->matrixB);
    free(ctx);
}

/**
 * @brief Performs matrix multiplication on the npu 
 * 
 * @param To - The type of the output matrix 
 * @param Ti1 - The type of the first input matrix (inferred automatically) 
 * @param Ti2 - The type of the second input matrix (inferred automatically) 
 * @param num_rows_a The number of rows in the first input mat
 * @param num_cols_a The number of columns in the first input mat
 * @param num_cols_b The number of columns in the second input mat
 * @param a The data of the first input matrix 
 * @param b The data of the second input matrix 
 * 
 * @return _matmul_ctx<To> that has inside the pointer to the result of the matmul.
 * 
 * @note The shape of the result is (num_rows_a, num_cols_b)
 */
template<typename To, typename Ti1, typename Ti2> 
tensor_result matmul_npu(
    uint32_t num_rows_a,
    uint32_t num_cols_a,
    uint32_t num_cols_b,
    Ti1* a,
    Ti2* b
) {

    _matmul_ctx* ctx = make_matmul(
        num_rows_a, num_cols_a, num_cols_b, choose_matmul_type<To, Ti1, Ti2>()
    );

    set_matrix_data(&ctx->ctx, ctx->matrixA, &ctx->io_attr.A, a);
    set_matrix_data(&ctx->ctx, ctx->matrixB, &ctx->io_attr.B, b);
    rknn_matmul_run(ctx->ctx);

    tensor_result result(ctx->ctx, ctx->matrixC);
    free_matmul(ctx);

    return result;

}


/**
 * @brief Performs matrix multiplication on the npu 
 * 
 * @param num_rows_a The number of rows in the first input mat
 * @param num_cols_a The number of columns in the first input mat
 * @param num_cols_b The number of columns in the second input mat
 * @param type The matmul type flag
 * @param a The data of the first input matrix 
 * @param b The data of the second input matrix 
 * 
 * @return _matmul_ctx<To> that has inside the pointer to the result of the matmul.
 * 
 * @note The shape of the result is (num_rows_a, num_cols_b)
 */
tensor_result matmul_npu(
    uint32_t num_rows_a,
    uint32_t num_cols_a,
    uint32_t num_cols_b,
    _rknn_matmul_type type,
    void* a,
    void* b
) {

    _matmul_ctx* ctx = make_matmul(
        num_rows_a, num_cols_a, num_cols_b, type
    );

    set_matrix_data(&ctx->ctx, ctx->matrixA, &ctx->io_attr.A, a);
    set_matrix_data(&ctx->ctx, ctx->matrixB, &ctx->io_attr.B, b);
    rknn_matmul_run(ctx->ctx);

    tensor_result result(ctx->ctx, ctx->matrixC);
    free_matmul(ctx);

    return result;

}


#endif

