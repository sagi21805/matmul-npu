#include "api_wrapper/matmul_api.hpp"
#include <memory>

template <typename T>
class Matrix {
        
    private:

        rknn_tensor_mem* tensor_mem;

    public: 

        int rows, cols;
        T* data; 

        Matrix() = default;

        Matrix(int rows, int cols, T* data) 
        : tensor_mem(nullptr), rows(rows), cols(cols), data(data) {}

        Matrix(rknn_tensor_mem* tensor_mem, int rows, int cols, T* data) 
        : tensor_mem(tensor_mem), rows(rows), cols(cols), data(data) {}
        
        template<typename To, typename Ti>
        Matrix<To> matmul(Matrix<Ti> mat) {
            rknn_tensor_mem* tensor_mem = matmul_npu<To, T, Ti>(
                this->rows, this->cols, mat.cols, this->data, mat.data
            );
            
            return Matrix<To>(tensor_mem, rows, cols, (To*) tensor_mem->virt_addr); 
    } 



};

