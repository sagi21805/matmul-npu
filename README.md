# Overview

This library designed to perform matrix multiplication on a Neural Processing Unit (NPU) using the [Rockchip RKNN toolkit2](https://github.com/airockchip/rknn-toolkit2). licensed under the BSD 3-Clause License.

## Installation
  To install the dependencies of this project, please use the `install.sh` file.
  (you may have to `chmod +x` it)

## Usage
```c++
#include "matmul_npu.hpp"
#include <iostream>
#include <vector>

int main() {

    int rows = 32;
    int cols = 32;

    std::vector<float16> a(rows*cols, 0.2);

    std::vector<float16> b(rows*cols, 0.3);

    _matmul_ctx<float32>* ctx = matmul_npu<float32>(rows, cols, rows, a.data(), b.data());

    std::cout << "The first item of the matrix: \n";
    std::cout << ctx->result[0] << "\n";
}
```

<br> Also see the `example.cpp` file which can be compiled using the `Makefile` in the repo. <br>


## Features

This library provides a convenient interface for setting up, executing, and managing matrix multiplication operations on NPUs,
supporting various data types such as `float16`, and `int8_t` as input matrices and `float16`, `int8`, and `float32` as output matrices.

- Supports multiple data types for input and output matrices.
- Simplifies NPU memory management.
- Provides utility functions to set matrix data and free resources.
- Performs efficient matrix multiplication on NPUs.

## Future additions 
  - Compatibility with the opencv Mat.
  - More operations on the NPU. (Dot Product, convolution, etc..)
  - Rust and Python Bindings.
