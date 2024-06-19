# Define the compiler
CXX = g++

# Define the source file
SRC = matmul-test.cpp

# Define the output executable
TARGET = matmul-test	

# Define the include directories
INC_DIRS = -I/usr/local/include/rknpu -I/usr/include/aarch64-linux-gnu/

# Define the library directories
LIB_DIRS = -L/usr/lib/aarch64-linux-gnu/

# Define the libraries to link against
LIBS = -lrknnrt -lcblas

# Define the compilation flags
CXXFLAGS = $(INC_DIRS) $(LIB_DIRS) $(LIBS)

# Define the rule to build the target
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS)

# Define the rule to clean up generated files
.PHONY: clean
clean:
	rm -f $(TARGET)

# Include dependencies (optional, useful if you have header files)
# DEP = $(SRC:.cpp=.d)
# -include $(DEP)
