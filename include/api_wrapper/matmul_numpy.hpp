#include "matmul_api.hpp"
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

void matmul_deleter(void* result) {
    tensor_result* r = static_cast<tensor_result*>(result);
    // Perform cleanup actions
    rknn_destroy_mem(r->ctx, r->resultMatrix);
    rknn_matmul_destroy(r->ctx);
    delete r;  // Free the struct itself

}

template<typename To, typename Ti1, typename Ti2>
py::array_t<To> matmul_numpy(py::array_t<Ti1> a, py::array_t<Ti2> b) {

    py::buffer_info a_info = a.request();

    py::buffer_info b_info = b.request();

    if (a_info.ndim != 2 || b_info.ndim != 2) {
        std::runtime_error("Matrices must be 2D");
    }

    tensor_result r = matmul_npu<To, Ti1, Ti2>(
        a_info.shape[0], 
        a_info.shape[1], 
        b_info.shape[1], 
        (Ti1*) a_info.ptr, 
        (Ti2*) b_info.ptr
    );

    tensor_result* heap_result = new tensor_result(r.ctx, r.resultMatrix); 

    py::capsule free_when_done((void*) heap_result, matmul_deleter);

    return py::array_t<To>(
        {a_info.shape[0], b_info.shape[1]}, 
        {b_info.shape[1] * sizeof(To), sizeof(To)}, 
        (To*) heap_result->resultMatrix->virt_addr, free_when_done
    );


}

