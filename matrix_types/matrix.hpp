#include "api_wrapper/matmul_api.hpp"
#include <memory>

template <typename T>
class Matrix {
        
    private:

        rknn_tensor_mem* tensor_mem;
        rknn_context ctx;

    public: 

        int rows, cols;
        T* data; 

        Matrix() = default;

        Matrix(int rows, int cols, T* data) 
        : tensor_mem(nullptr), ctx(0), rows(rows), cols(cols), data(data) {}

        Matrix(rknn_tensor_mem* tensor_mem, rknn_context ctx, int rows, int cols, T* data) 
        : tensor_mem(tensor_mem), ctx(ctx), rows(rows), cols(cols), data(data) {}

        ~Matrix() {
            rknn_destroy_mem(ctx, tensor_mem);
            rknn_matmul_destroy(ctx);
        }
        
        template<typename To, typename Ti>
        Matrix<To> matmul(Matrix<Ti> mat) {
            tensor_result result = matmul_npu<To, T, Ti>(
                this->rows, this->cols, mat.cols, this->data, mat.data
            );
            
            return Matrix<To>(
                result.resultMatrix, 
                result.ctx,
                rows, mat.cols, 
                (To*) result.resultMatrix->virt_addr
            ); 
    } 



};

