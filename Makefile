# Define the compiler
CXX = g++

# Define the source file
SRC = example.cpp

# Define the include directories
RKNPU_INC_DIR = -I/usr/local/include/rknpu/

OPENCV_INC_DIR = -I/usr/local/include/opencv4/

CURRENT_INC_DIR = -I./

LIB_DIR = -L/usr/local/lib

# Define the libraries to link against
LIBS = -lrknnrt -fopenmp -lopencv_core

# Define the compilation flags
CXX_INCLUDE_FLAGS = $(RKNPU_INC_DIR) $(OPENCV_INC_DIR) $(CURRENT_INC_DIR)

CXX_LIB_FLAGS =  $(LIB_DIR) $(LIBS) 

CXXFLAGS = -Wall -O3

# Define the rule to build the target
example: example.cpp
	$(CXX) example.cpp -o example $(CXX_INCLUDE_FLAGS) $(CXX_LIB_FLAGS) $(CXXFLAGS)

test: test.cpp
	$(CXX) test.cpp -o test $(CXX_INCLUDE_FLAGS) $(CXX_LIB_FLAGS) $(CXXFLAGS)

opencv: example_opencv.cpp
	$(CXX) example_opencv.cpp -o example_opencv $(CXX_INCLUDE_FLAGS) $(CXX_LIB_FLAGS) $(CXXFLAGS)
# Define the rule to clean up generated files
.PHONY: clean
clean:
	rm -f example test example_opencv