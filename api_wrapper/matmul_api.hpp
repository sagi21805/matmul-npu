#ifndef MATMUL_NPU
#define MATMUL_NPU

#include <rknpu/rknn_matmul_api.h>
#include <type_traits>
#include <iostream>
#include <cstring>

typedef float float32;
typedef __fp16 float16;

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
    if constexpr (
        std::is_same_v<float16, Ti1> && 
        std::is_same_v<float16, Ti2> && 
        std::is_same_v<float16, To>) {
        return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT16;
    } else if constexpr (
        std::is_same_v<float16, Ti1> && 
        std::is_same_v<float16, Ti2> && 
        std::is_same_v<float32, To>)  {
        return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT32;
    } else if constexpr (
        std::is_same_v<int8_t, Ti1> && 
        std::is_same_v<int8_t, Ti2> && 
        std::is_same_v<int8_t, To>)  {
        return RKNN_INT8_MM_INT8_TO_INT8;
    } else if constexpr (
        std::is_same_v<int8_t, Ti1> && 
        std::is_same_v<int8_t, Ti2> && 
        std::is_same_v<int32_t, To>)  {
        return RKNN_INT8_MM_INT8_TO_INT32;
    } else {
        std::cout << "unsupported combination of types:\n";
        std::cout << "please enter types from avilable types\n";
        std::cout << "1. float16, float16, float16\n";
        std::cout << "2. float16, float16, float32\n";
        std::cout << "5. int8_t, int8_t, int32_t\n";
        std::cout << "6. int8_t, int8_t, int8_t\n";
        abort();
    }

}

/**
 * Struct that wraps all the built in rknn types 
 * and contains the result pointer
 * 
 * @param To The type of the output matrix
 */
template<typename To> 
struct _matmul_ctx {
    rknn_context ctx;
    rknn_matmul_info info;
    rknn_matmul_io_attr io_attr;
    rknn_tensor_mem* matrixA;
    rknn_tensor_mem* matrixB;
    rknn_tensor_mem* matrixC;

    To* result;

};

/**
 * @brief ## __Create a matmul operation for the npu__
 * 
 * @param To The type of the output matrix
 * 
 * @return _matmul_ctx with the currect context for the rknn_matmul_run function
 */
template<typename To> 
_matmul_ctx<To>* make_matmul(
    int32_t num_rows_a, int32_t num_cols_a, int32_t num_cols_b, _rknn_matmul_type type
    ) {

    /* create a matmul_ctx struct */
    _matmul_ctx<To>* matmul_ctx = (_matmul_ctx<To>*)malloc(sizeof(_matmul_ctx<To>));

    /* set all field to zero */
    memset(matmul_ctx, 0, sizeof(_matmul_ctx<To>));

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

    // set the result pointer to point on where the result will be
    matmul_ctx->result = (To*)matmul_ctx->matrixC->virt_addr;

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
template<typename Ti> 
void set_matrix_data(
    rknn_matmul_ctx* ctx, 
    rknn_tensor_mem* mem, 
    rknn_matmul_tensor_attr* attr, 
    const Ti* data ) {

    size_t size = mem->size / sizeof(Ti);
    Ti* ptr = (Ti*)mem->virt_addr;
    for (size_t i = 0; i < size; ++i) {
        ptr[i] = data[i];
    }
    rknn_matmul_set_io_mem(*ctx, mem, attr);
}

/**
 * @brief Free the matrices tensors 
 * 
 * @param To The type of the input matrix
 * @param ctx The context of the matmul operation
 */
template<typename To> 
void free_matmul(_matmul_ctx<To>* ctx) {
    rknn_destroy_mem(ctx->ctx, ctx->matrixA);
    rknn_destroy_mem(ctx->ctx, ctx->matrixB);
    rknn_destroy_mem(ctx->ctx, ctx->matrixC);

    rknn_matmul_destroy(ctx->ctx);

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
_matmul_ctx<To>* matmul_npu(
    uint32_t num_rows_a,
    uint32_t num_cols_a,
    uint32_t num_cols_b,
    Ti1* a,
    Ti2* b
) {

    _matmul_ctx<To>* ctx = make_matmul<To>(
        num_rows_a, num_cols_a, num_cols_b, choose_matmul_type<To, Ti1, Ti2>()
    );

    set_matrix_data(&ctx->ctx, ctx->matrixA, &ctx->io_attr.A, a);
    set_matrix_data(&ctx->ctx, ctx->matrixB, &ctx->io_attr.B, b);
    rknn_matmul_run(ctx->ctx);

    return ctx;

}


#endif

