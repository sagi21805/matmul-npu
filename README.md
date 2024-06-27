# Overview

This library designed to perform matrix multiplication on a Neural Processing Unit (NPU) using the [Rockchip RKNN toolkit2](https://github.com/airockchip/rknn-toolkit2). licensed under the BSD 3-Clause License.

## Installation
  To install the dependencies of this project, please use the `install.sh` file.
  If you want to use the opencv and or python features, add the --opencv=true and or --python=true accordingly.

## Usage

### C++
```c++
#include "matrix_types/matrix.hpp"
#include "matrix_types/opencv_mat.hpp"
#include <iostream>
#include <vector>

int main() {

    int rows = 4096;
    int cols = 4096;

    std::vector<int8_t> a(rows*cols, 2);

    std::vector<int8_t> b(rows*cols, 3);

    // without opencv
    Matrix<int8_t> A(rows, cols, a.data());
    Matrix<int8_t> B(rows, cols, b.data());

    Matrix<int32_t> C = A.matmul<int32_t>(B);

    std::cout << "The first item of the matrix: ";
    std::cout << C.data[0] << "\n";

    //with opencv
    MatNpu A(rows, cols, CV_8S, a.data());
    MatNpu B(rows, cols, CV_8S, b.data());

    MatNpu C = A.matmul(B, CV_32S);

    std::cout << "The first item of the matrix: ";
    std::cout << C.at<int32_t>(0, 0) << "\n";
}
```

### Python
```python
import matnpu
import numpy as np

# Generate some random data
a = np.random.randint(low=-128, high=127, size=(320, 320), dtype=np.int8)
b = np.random.randint(low=-128, high=127, size=(320, 320), dtype=np.int8)

# npu calculation
npu_matmul = matnpu.matmul_i32(a, b)
print(npu_matmul)

```

<br> Also see the `example.cpp` & `example_opencv.cpp` files which can be compiled using the `Makefile` in the repo. or the `example.py` for python <br>


## Features

This library provides a convenient interface for setting up, executing, and managing matrix multiplication operations on NPUs,
supporting various data types such as `float16`, and `int8_t` as input matrices and `float16`, `int8`, and `float32` as output matrices.

- Supports multiple data types for input and output matrices.
- Simplifies NPU memory management.
- Provides utility functions to set matrix data and free resources.
- Performs efficient matrix multiplication on NPUs.
- Extention of the [OpenCV](https://github.com/opencv/opencv) Mat
- Python bindings

## Future additions 
  - More operations on the NPU. (Dot Product, convolution, etc..)
  - Rust Bindings.
