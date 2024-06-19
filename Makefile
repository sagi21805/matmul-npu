# Define the compiler
CXX = g++

# Define the source file
SRC = example.cpp

# Define the output executable
TARGET = example

# Define the include directories
INC_DIRS = -I/usr/local/include/rknpu

# Define the libraries to link against
LIBS = -lrknnrt

# Define the compilation flags
CXXFLAGS = $(INC_DIRS) $(LIB_DIRS) $(LIBS)

# Define the rule to build the target
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS)

# Define the rule to clean up generated files
.PHONY: clean
clean:
	rm -f $(TARGET)