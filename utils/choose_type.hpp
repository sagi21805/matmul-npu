#ifndef CHOOSE_TYPE
#define CHOOSE_TYPE

#include <rknpu/rknn_matmul_api.h>
#include <opencv4/opencv2/opencv.hpp>

typedef __fp16 float16;
typedef float float32;


_rknn_matmul_type choose_matmul_type(int input1, int input2, int output) {
    
    if (
        input1 == CV_16F &&
        input2 == CV_16F &&
        output == CV_16F ) { 
        return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT16; 
    } else if (
        input1 == CV_16F &&
        input2 == CV_16F &&
        output == CV_32F ) { 
        return RKNN_FLOAT16_MM_FLOAT16_TO_FLOAT32; 
    } else if (
        input1 == CV_16F &&
        input2 == CV_8S &&
        output == CV_16F ) { 
        return RKNN_FLOAT16_MM_INT8_TO_FLOAT16; 
    } else if (
        input1 == CV_8S &&
        input2 == CV_8S &&
        output == CV_8S) { 
        return RKNN_INT8_MM_INT8_TO_INT8; 
    } else if (
        input1 == CV_8S &&
        input2 == CV_8S &&
        output == CV_32S) { 
        return RKNN_INT8_MM_INT8_TO_INT32; 
    } else {
        std::cout << "unsupported combination of types:\n";
        std::cout << "please enter types from avilable types\n";
        std::cout << "1. float16, float16, float16\n";
        std::cout << "2. float16, float16, float32\n";
        std::cout << "3. float16, int8_t, float16\n";
        std::cout << "4. int8_t, int8_t, int8_t\n";
        std::cout << "4. int8_t, int8_t, int32_t\n";
        abort();
    }

}

#endif