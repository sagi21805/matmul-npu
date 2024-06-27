// bindings.cpp
#include <pybind11/pybind11.h>
#include "api_wrapper/matmul_numpy.hpp"
#include "utils/pybind11_float16.hpp"

namespace py = pybind11;



PYBIND11_MODULE(matnpu, m) {
  
    m.def("matmul_f16", &matmul_numpy<float16, float16, float16>,
        "A function that multiplies two matrices on the npu",
        py::arg("a"), py::arg("b") 
    );
    m.def("matmul_f32", &matmul_numpy<float32, float16, float16>, 
        "A function that multiplies two matrices on the npu",
        py::arg("a"), py::arg("b") 
    );
    m.def("matmul_f16", &matmul_numpy<float16, float16, int8_t>,
        "A function that multiplies two matrices on the npu",
        py::arg("a"), py::arg("b") 
    );
    m.def("matmul_i8", &matmul_numpy<int8_t, int8_t, int8_t>, 
        "A function that multiplies two matrices on the npu",
        py::arg("a"), py::arg("b") 
    );
    m.def("matmul_i32", &matmul_numpy<int32_t, int8_t, int8_t>,
        "A function that multiplies two matrices on the npu",
        py::arg("a"), py::arg("b") 
    );

}
