#include "matmul_utils.hpp"

_rknn_matmul_ctx* make_matmul(int32_t M, int32_t K, int32_t N, _rknn_matmul_type flag) {

    _rknn_matmul_ctx* ctx = (_rknn_matmul_ctx*)malloc(sizeof(_rknn_matmul_ctx));

    memset(ctx, 0, sizeof(_rknn_matmul_ctx));

    ctx->info.M             = M;
    ctx->info.K             = K;
    ctx->info.N             = N;
    ctx->info.type          = flag;
    ctx->info.AC_layout     = 1;
    ctx->info.B_layout      = 1;

    int ret = rknn_matmul_create(&ctx->ctx, &ctx->info, &ctx->io_attr);
    if (ret < 0) {
        printf("rknn_matmul_create fail! ret=%d\n", ret);
        abort();
    }

    // Create A
    ctx->matrixA = rknn_create_mem(ctx->ctx, ctx->io_attr.A.size);
    ctx->matrixB = rknn_create_mem(ctx->ctx, ctx->io_attr.B.size);
    ctx->matrixC = rknn_create_mem(ctx->ctx, ctx->io_attr.C.size);

    ctx->M = M;
    ctx->K = K;
    ctx->N = N;

    ctx->matrixA_ptr = (float16*)ctx->matrixA->virt_addr;
    ctx->matrixB_ptr = (float16*)ctx->matrixB->virt_addr;
    ctx->matrixC_ptr = (float*)ctx->matrixC->virt_addr;

    rknn_matmul_set_io_mem(ctx->ctx, ctx->matrixA, &ctx->io_attr.A);
    rknn_matmul_set_io_mem(ctx->ctx, ctx->matrixB, &ctx->io_attr.B);
    rknn_matmul_set_io_mem(ctx->ctx, ctx->matrixC, &ctx->io_attr.C);

    return ctx;
}
